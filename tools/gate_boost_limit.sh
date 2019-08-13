# Effect a noise gate
# Boost the signal
# Limit the signal
# TODO: introduces a delay into the sound file

tmp_file_name=$(date "+%s")

conv_outfile=/tmp/orig_snd_${tmp_file_name}.f32
orig_sr=$(soxi -r "$INFILE")
sox "$INFILE" -c 1 -t f32 "$conv_outfile"

gate_outfile=/tmp/local_gated_snd_${tmp_file_name}.f32
# noise gate
INFILE="$conv_outfile" \
OUTFILE="$gate_outfile" \
tools/bin/noise_gate_mono_f32
rm "$conv_outfile"

# boost
boost_outfile="/tmp/local_gated_boosted_SCALAR=${SCALAR}_snd_${tmp_file_name}.f32"
PYTHONPATH=".:$PYTHONPATH" \
INFILE="$gate_outfile" \
OUTFILE=$boost_outfile \
python3 test/scale_mono_f32.py
rm "$gate_outfile"

# limit

limit_outfile="${boost_outfile%.*}_limited.f32"
INFILE=$boost_outfile \
OUTFILE=$limit_outfile \
tools/bin/limit_mono_f32
rm "$boost_outfile"

# give 3dB headroom
headroom_outfile="${limit_outfile%.*}_headroom.f32"
PYTHONPATH=".:$PYTHONPATH" \
INFILE=$limit_outfile \
OUTFILE=$headroom_outfile \
SCALAR=0.7 \
python3 test/scale_mono_f32.py
rm "$limit_outfile"

outfile="${INFILE%.*}_bgbl.wav"
echo "$outfile"
sox -r $orig_sr -c 1 "$headroom_outfile" "$outfile"
rm "$headroom_outfile"
