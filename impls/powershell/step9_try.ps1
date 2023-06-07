$ErrorActionPreference = "Stop"

Import-Module $PSScriptRoot/types.psm1
Import-Module $PSScriptRoot/reader.psm1
Import-Module $PSScriptRoot/printer.psm1
Import-Module $PSScriptRoot/env.psm1
Import-Module $PSScriptRoot/core.psm1

# READ
function READ([String] $str) {
    return read_str($str)
}

# EVAL
function starts_with($lst, $sym) {
    if ($lst.values.Count -ne 2) { return $false }
    $a0 = $lst.nth(0)
    return (symbol? $a0) -and ($a0.value -ceq $sym)
}
function qq_loop($elt, $acc) {
    if ((list? $elt) -and (starts_with $elt "splice-unquote")) {
        return (new-list @((new-symbol "concat"), $elt.nth(1), $acc))
    } else {
        return (new-list @((new-symbol "cons"), (quasiquote $elt), $acc))
    }
}
function qq_foldr($xs) {
    $acc = new-list @()
    for ( $i = $xs.Count - 1; $i -ge 0; $i-- ) {
        $acc = qq_loop $xs[$i] $acc
    }
    return $acc
}
function quasiquote($ast) {
    if ($ast -eq $null) { return $ast }
    switch ($ast.GetType().Name) {
        "Symbol"    { return (new-list @((new-symbol "quote"), $ast)) }
        "HashMap"   { return (new-list @((new-symbol "quote"), $ast)) }
        "Vector"    { return (new-list @((new-symbol "vec"), (qq_foldr $ast.values))) }
        "List"      {
            if (starts_with $ast "unquote") {
                return $ast.values[1]
            } else {
                return qq_foldr $ast.values
            }
        }
        default     { return $ast }
    }
}

function macro?($ast, $env) {
    return (list? $ast) -and
           (symbol? $ast.nth(0)) -and
           $env.find($ast.nth(0)) -and
           $env.get($ast.nth(0)).macro
}

function macroexpand($ast, $env) {
    while (macro? $ast $env) {
        $mac = $env.get($ast.nth(0)).fn
        $margs = $ast.rest().values
        $ast = &$mac @margs
    }
    return $ast
}

function eval_ast($ast, $env) {
    if ($ast -eq $null) { return $ast }
    switch ($ast.GetType().Name) {
        "Symbol"  { return $env.get($ast) }
        "List"    { return new-list ($ast.values | ForEach { EVAL $_ $env }) }
        "Vector"  { return new-vector ($ast.values | ForEach { EVAL $_ $env }) }
        "HashMap" {
            $hm = new-hashmap @()
            foreach ($k in $ast.values.Keys) {
                $hm.values[$k] = EVAL $ast.values[$k] $env
            }
            return $hm
        }
        default   { return $ast }
    }
}

function EVAL($ast, $env) {
  while ($true) {
    #Write-Host "EVAL $(pr_str $ast)"
    if (-not (list? $ast)) {
        return (eval_ast $ast $env)
    }

    $ast = (macroexpand $ast $env)
    if (-not (list? $ast)) {
        return (eval_ast $ast $env)
    }
    if (empty? $ast) { return $ast }

    $a0, $a1, $a2 = $ast.nth(0), $ast.nth(1), $ast.nth(2)
    switch -casesensitive ($a0.value) {
        "def!" {
            return $env.set($a1, (EVAL $a2 $env)) 
        }
        "let*" {
            $let_env = new-env $env
            for ($i=0; $i -lt $a1.values.Count; $i+=2) {
                $_ = $let_env.set($a1.nth($i), (EVAL $a1.nth(($i+1)) $let_env))
            }
            $env = $let_env
            $ast = $a2  # TCO
        }
        "quote" {
            return $a1
        }
        "quasiquoteexpand" {
            return (quasiquote $a1)
        }
        "quasiquote" {
            $ast = quasiquote $a1
        }
        "defmacro!" {
            $m = EVAL $a2 $env
            $m.macro = $true
            return $env.set($a1, $m)
        }
        "macroexpand" {
            return (macroexpand $a1 $env)
        }
        "try*" {
            try {
                return EVAL $a1 $env
            } catch {
                if ($a2 -and ($a2.nth(0).value -ceq "catch*")) {
                    if ($_.Exception.GetType().Name -eq "MalException") {
                        $e = @($_.Exception.object)
                    } else {
                        $e = @($_.Exception.Message)
                    }
                    return (EVAL $a2.nth(2) (new-env $env @($a2.nth(1)) $e))
                } else {
                    throw
                }
            }
        }
        "do" {
            if ($ast.values.Count -gt 2) {
                $middle = new-list $ast.values[1..($ast.values.Count-2)]
                $_ = eval_ast $middle $env
            }
            $ast = $ast.last()  # TCO
        }
        "if" {
            $cond = (EVAL $a1 $env)
            if ($cond -eq $null -or
                ($cond -is [Boolean] -and $cond -eq $false)) {
                $ast = $ast.nth(3)  # TCO
            } else {
                $ast = $a2  # TCO
            }
        }
        "fn*" {
            # Save EVAL into a variable that will get closed over
            $feval = Get-Command EVAL
            $fn = {
                return (&$feval $a2 (new-env $env $a1.values $args))
            }.GetNewClosure()
            return new-malfunc $a2 $a1.values $env $fn
        }
        default {
            $el = (eval_ast $ast $env)
            $f, $fargs = $el.first(), $el.rest().values
            if (malfunc? $f) {
                $env = (new-env $f.env $f.params $fargs)
                $ast = $f.ast  # TCO
            } else {
                return &$f @fargs
            }
        }
    }
  }
}

# PRINT
function PRINT($exp) {
    return pr_str $exp $true
}

# REPL
$repl_env = new-env

function REP([String] $str) {
    return PRINT (EVAL (READ $str) $repl_env)
}

# core.EXT: defined using PowerShell
foreach ($kv in $core_ns.GetEnumerator()) {
    $_ = $repl_env.set((new-symbol $kv.Key), $kv.Value)
}
$_ = $repl_env.set((new-symbol "eval"), { param($a); (EVAL $a $repl_env) })
$_ = $repl_env.set((new-symbol "*ARGV*"), (new-list $args[1..$args.Count]))

# core.mal: defined using the language itself
$_ = REP('(def! not (fn* (a) (if a false true)))')
$_ = REP('(def! load-file (fn* (f) (eval (read-string (str "(do " (slurp f) "\nnil)")))))')
$_ = REP("(defmacro! cond (fn* (& xs) (if (> (count xs) 0) (list 'if (first xs) (if (> (count xs) 1) (nth xs 1) (throw `"odd number of forms to cond`")) (cons 'cond (rest (rest xs)))))))")


if ($args.Count -gt 0) {
    $_ = REP('(load-file "' + $args[0] + '")')
    exit 0
}

while ($true) {
    Write-Host "user> " -NoNewline
    $line = [Console]::ReadLine()
    if ($line -eq $null) {
        break
    }
    try {
        Write-Host (REP($line))
    } catch {
        if ($_.Exception.GetType().Name -eq "MalException") {
            Write-Host "Exception: $(pr_str $_.Exception.object)"
        } else {
            Write-Host "Exception: $($_.Exception.Message)"
        }
    }
}
