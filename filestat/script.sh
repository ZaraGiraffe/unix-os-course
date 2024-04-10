
if [ $# -lt 1 ]; then
	echo "You should provide directory"
	exit
fi

OUTPUT="data.json"
counter=0
dir=$1

for file in $(sudo find $dir -type f -o -type d)
do
	size=$(stat -c%s $file)
	echo "{'name': '$file', 'size': $size}" >> $OUTPUT
	echo "i: $couter"
	((couter++))
done

