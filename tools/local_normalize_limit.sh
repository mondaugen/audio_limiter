# Normalize a file using local normalization and then limit the result

[ -z $W ] && W=20000
tmp_file_name=$(date "+%s")

orig_sr=$(soxi -r "$INFILE")
sox "$INFILE" -c 1 -t f32 /tmp/orig_snd_${tmp_file_name}.f32

# normalize
PYTHONPATH=".:$PYTHONPATH" \
INFILE=/tmp/orig_snd_${tmp_file_name}.f32 \
OUTFILE=/tmp/local_norm_snd_${tmp_file_name}.f32 \
W=$W \
python3 tools/local_normalize.py

# limit

INFILE=/tmp/local_norm_snd_${tmp_file_name}.f32 \
OUTFILE=/tmp/limited_snd_${tmp_file_name}.f32 \
tools/bin/limit_mono_f32

outfile=$(printf "${INFILE}_lnl_W=%d.wav" $W)
echo "$outfile"
sox -r $orig_sr -c 1 /tmp/limited_snd_${tmp_file_name}.f32 "$outfile"
