#!/usr/bin/env bash
set -euo pipefail

BIN="${1:-./build/cesta_basica}"

echo "==> 1. Testando modo --demo"
DEMO_OUT="$("$BIN" --demo)"
grep -q "Arroz Tipo 1" <<<"$DEMO_OUT"
grep -q "AUMENTO" <<<"$DEMO_OUT"
grep -q "QUEDA" <<<"$DEMO_OUT"
grep -q "ESTAVEL" <<<"$DEMO_OUT"
echo "[OK] Modo --demo validado"

echo "==> 2. Testando modo interativo com entrada invalida recuperada"
INTERACTIVE_OUT="$(
  "$BIN" <<'EOF'
Cafe 500g
abc
-10
20.00
24.00
Acucar 1kg
5.00
4.50
Leite 1L
6,00
6,00
EOF
)"
grep -q "Valor invalido" <<<"$INTERACTIVE_OUT"
grep -q "Cafe 500g" <<<"$INTERACTIVE_OUT"
grep -q "+20.00%" <<<"$INTERACTIVE_OUT"
grep -q -- "-10.00%" <<<"$INTERACTIVE_OUT"
grep -q "1 AUMENTO | 1 QUEDA | 1 ESTAVEL" <<<"$INTERACTIVE_OUT"
echo "[OK] Modo interativo e validacao de entrada OK"
