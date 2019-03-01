GUITAR_SAMPLES_DIR=${HOME}/Documents/sounds/mums/mums/disc1/GUITARS/ELECTRIC_GUITAR/ELECTRIC_GUITAR

FILES=(
    'ELECTRIC_GUITAR_E2.mp4'
    'ELECTRIC_GUITAR_B2.mp4'
    'ELECTRIC_GUITAR_F#3.mp4'
    'ELECTRIC_GUITAR_C#4.mp4'
    'ELECTRIC_GUITAR_G#4.mp4'
    'ELECTRIC_GUITAR_D#5.mp4'
    'ELECTRIC_GUITAR_A#5.mp4'
)

# Trim length because there's some error in the ALS files near the ends
LEN_SEC=10

cmdline='ffmpeg '

n_files="${#FILES[*]}"

for file in ${FILES[*]}; do
    cmdline+="-i $GUITAR_SAMPLES_DIR/$file "
done

cmdline+="-filter_complex '"
for i in `seq 0 "$(($n_files-1))"`; do
    cmdline+="[$i]adelay=$((1000*$i+1))|$((1000*$i+1))[d$i],"
done
for i in `seq 0 "$(($n_files-1))"`; do
    cmdline+="[d$i]"
done
cmdline+="amix=inputs=${#FILES[*]},"
cmdline+="atrim=start=0:end=$LEN_SEC"
cmdline+="' "
#cmdline+="-f pulse \"HI\""

echo "$cmdline"
bash -c "$cmdline /tmp/guitar.flac"&
#bash -c "$cmdline -f pulse \"HI\""
