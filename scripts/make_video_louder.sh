# extracts the audio from a movie and makes it louder using
# tools/gate_boost_limit.sh
# TODO: the number of seconds since epoch is used to make a unique file name, so
# you can't run this is parallel

tmp_file_name=$(date "+%s")

# extract audio
audio_only_outfile=/tmp/audio_only_${tmp_file_name}.wav
ffmpeg -i "$INFILE" -vn "$audio_only_outfile"

# gate / boost / limit
INFILE="$audio_only_outfile" bash tools/gate_boost_limit.sh

# make a silent movie
video_only_outfile=/tmp/video_only_${tmp_file_name}.${INFILE##*.}
ffmpeg -i "$INFILE" -codec copy -an "$video_only_outfile"

# combine the louder audio with this movie
ffmpeg -i "$video_only_outfile" -i "${audio_only_outfile%.wav}_bgbl.wav" \
    -shortest -c:v copy -c:a ac3 -b:a 256k "${INFILE%.*}_louder.${INFILE##*.}"

