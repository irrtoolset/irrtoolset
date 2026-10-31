#!/bin/sh
cd "$(dirname "$0")"
OUT="${OUTDIR:-../build/examples}"
mkdir -p "$OUT"
# Generate the IOS example router config
echo building IOS example config into: $OUT/ios-example-output.txt
"${RTCONFIG:-../build/bin/rtconfig}" -h whois.radb.net -f private_irr_objects.txt -p 43 -protocol rawhoisd -config cisco -supress_martian -cisco_peer_templates -cisco_use_prefix_lists -cisco_empty_lists < ios-example-input.cfg > "$OUT/ios-example-output.txt"

# Generate the IOS XR example router config
echo building IOS XR example config into: $OUT/xr-example-output.txt
"${RTCONFIG:-../build/bin/rtconfig}" -h whois.radb.net -f private_irr_objects.txt -p 43 -protocol rawhoisd -config ciscoxr  -supress_martian < xr-example-input.cfg > "$OUT/xr-example-output.txt"

# Generate the JunOS example router config
echo building JunOS example config into: $OUT/junos-example-output.txt
"${RTCONFIG:-../build/bin/rtconfig}" -h whois.radb.net -f private_irr_objects.txt -p 43 -protocol rawhoisd -config junos  -junos_replace -junos_and_not_or -supress_martian < junos-example-input.cfg > "$OUT/junos-example-output.txt"
