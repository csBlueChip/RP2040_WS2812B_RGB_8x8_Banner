dir=zxorigins/
dirh=$dir/html/
dirt=$dir/tmp/

master=${dir}zxorigins.h

#rm -rf $dir
mkdir -p "$dirh" "$dirt"

#-------------------------------------------------------------------------------
# patch header files
fixup() {
	while IFS= read -r l || [[ -n "$l" ]] ; do
		echo "# Fixing: $l"
		ifh=`grep FONT_ "$l" | sed 's/.*FONT_\(.*\)_BITMAP.*/\1/'`
		sed -i -e '1s/^/#ifndef  '$ifh'\n#define  '$ifh'\n\n#include  <stdint.h>\n\n/' \
		       -e 's/\\$/\\ (backslash)/' \
		       -e '$ a \\n#endif//'$ifh \
		"$l"
	done < <(grep -L ifndef ${dir}/*.h)
}

#-------------------------------------------------------------------------------
# create master font header
domaster() {
	local cfile="${master%.h}.c"
	rm -f $master $cfile

	echo "# DOMASTER"

	 >$master  echo      "#ifndef  ZX_ORIGINS_H_"
	>>$master  echo -e   "#define  ZX_ORIGINS_H_\n"

	while IFS= read -r j || [[ -n "$j" ]] ; do
		>>$master  echo "#include \"${j##*/}\""
	done < <(ls ${dir}*.h)

cat <<EOF >>$master

enum zxoID {
EOF

	 >"$cfile" echo "#include <stdlib.h>  // NULL"
	>>"$cfile" echo ""
	>>"$cfile" echo "#include \"${master}\""
	>>"$cfile" echo ""
	>>"$cfile" echo "const zxorigins_t  zxorigins[] = {"
	while IFS= read -r f || [[ -n "$f" ]] ; do

		while IFS= read -r n || [[ -n "$n" ]] ; do
			local fontn="`sed 's/.*: "\([^"]*\).*/\1/' <<<$n`"
			echo -ne "\r$fontn\e[K"

			local fonth="${dir}/${fontn}.h"
			local fonta="`grep FONT_ "$fonth" | sed 's/.*\(FONT_[^\[]*\).*/\1/'`"
			local fontd="`grep '#ifndef' "$fonth" | sed 's/^.* //'`"

			>>$cfile printf "#ifdef ${fontd}\n"
			>>$cfile printf "\t{ \"%s\"%*s, %-43s },\n" "$fontn" $((36-${#fontn})) "" "$fonta"
			>>$cfile printf "#endif\n"

			local fontc="${fontn}.c"
			>>"${cfile}.tmp"  printf "#ifdef  ${fontd}\n"
			>>"${cfile}.tmp"  printf "#\tinclude \"$fontc\"\n"
			>>"${cfile}.tmp"  printf "#endif\n"

			>>"${cfile}.enum"  printf "\t${fonta:5:-7},\n"

		done < <(grep '"name"' "$f")

	done < <(ls ${dir}/*.json)

cat <<EOF >>"${cfile}"
	{ NULL, NULL }
};

EOF

	cat "${cfile}.tmp" >>"${cfile}"
	rm "${cfile}.tmp"

cat <<EOF >>"${cfile}"

const int  zxoCnt = (sizeof(zxorigins) / sizeof(*zxorigins)) -1;

#include <stdint.h>

const uint8_t*  zxoFont (enum zxoID id)
{
	return zxorigins[id].font;
}
EOF

cat "${cfile}.enum" >> "${master}"

cat <<EOF >>"${master}"
};

#include  <stdint.h>

typedef
	struct zxorigins {
		const char*     name;
		const uint8_t*  font;
	}
zxorigins_t;

extern const zxorigins_t  zxorigins[];
extern const int          zxoCnt;

const uint8_t*  zxoFont (enum zxoID id) ;

#endif //ZX_ORIGINS_H_
EOF
	rm "${cfile}.enum"

	echo ""
}

#-------------------------------------------------------------------------------
convert() {

	echo "# CONVERT"

	while IFS= read -r h || [[ -n "$h" ]] ; do
		echo -ne "\r$h\e[K"

		local c="${h%%.*}.c"

		# remove first 4 and last 2 lines; remove 'static'
		sed -e "1,4d" \
		    -e "$(( $(wc -l < "$h") -(2-1) )),\$d" \
		    -e 's/^static //' \
		"$h" > "$c"

		sed -e 's/^static\(.*\)\].*/extern\1\];/' \
		    -e "/, 0x/d" \
		    -e "/^}/d" \
		-i "$h"

	done < <(ls ${dir}/*.h)

	echo ""
}

#-------------------------------------------------------------------------------
# create tags list
#
maketags() {
	local tagarr=($@)

	echo "# MAKETAGS..."

	# make tags file
	tagfile=${dir}/_tags.lst

	 >${tagfile}  echo    "Count : ${#tagarr[@]}"
	>>${tagfile}  echo    "tags+ : ${tagarr[@]}"
	>>${tagfile}  echo -n "tags- : "
	while IFS= read -r l || [[ -n $l ]] ; do
		>>${tagfile}  echo -n "${l%@*} "
	done < <(printf "%s\n" "${tagarr[@]}")
	>>${tagfile}  echo ""

	while IFS= read -r l || [[ -n $l ]] ; do
		>>${tagfile}  echo -e "\n${l/@/ [}]"
		grep -hB1 "[\",]${l%@*}" ${dir}/*.json \
		| grep -v -e '^--$' -e '"tags"' \
		| sed -e 's/.*: "\([^"]*\).*/\1/' -e 's/^/\t/' \
		>>${tagfile}
	done < <(printf "%s\n" "${tagarr[@]}")
}

#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#x=(futuristic@41 angular@26 aggressive@9 5-wide@19 narrow@21 sci-fi@20 family@31 unused@132 heavy@36 fun@14 friendly@17 bubble@6 demo-scene@2 conversion@7 sharp@26 adaptation@25 rustic@13 bold@56 ornate@5 clean@32 4-wide@1 quirky@5 italic@10 stencil@5 cutout@27 decorative@26 1960s@10 1970s@6 fave@15 gaming@10 horror@9 distressed@11 racing@7 oblique@15 small@13 short@14 advent@36 outdoors@9 1920s@11 contrast@17 outline@8 old@10 formal@16 square@17 space@19 wide@3 movie@12 magnetic@12 tall@9 script@9 flowing@17 chrome@4 woodblock@4 frontier@4 faux-eastern@5 rough@4 semi-serif@8 1930s@4 kinetic@12 flight@4 small-caps@12 brush@3 1980s@9 arcade@2 printer@2 1950s@1 ocr@3 1990s@2)
#maketags ${x[@]}
#convert
#domaster
#exit

# get the index
index=$dirh/_index.html
[[ -e $index ]] || wget https://damieng.com/typography/zx-origins/ -O $index

# extract the good bits, one line at a time
grep 'id="zx-origins-index"' $index \
	| sed 's_li><li_li>\n<li_g'  \
	| sed 's_^.*dex">__'         \
	| sed 's_</ul>.*__'          \
>list

#typeface and font counters
tcnt=0
fcnt=0
tagarr=()

# let's process some html with regex's :) ...
while IFS= read -r l || [[ -n $l ]] ; do
	                 # address bug in the `mcedit` syntax hiliter ---vv
	title=$(sed 's/^.*title"> *\([^<]*\)<.*/\1/' <<<$l)              #'
	echo -n "# $title : "

	# collect the tags
	tags=$(sed 's/^.*tags="\([^"]*\)".*/\1/' <<<$l)                  #"
	tarr=(`sed 's/,/ /g' <<<${tags}`)
	for ((i = 0;  i < ${#tarr[@]};  i++)) ; do
		for ((j = 0;  j < ${#tagarr[@]};  j++)) ; do
			si=${tagarr[$j]}
			id=${si%@*}
			if [[ "${tarr[$i]}" == "${id}" ]] ; then
				cnt=${si#*@}
				((cnt++))
				tagarr[$j]="$id@$cnt"
				j=999
			fi
		done
		[[ $j == 1000 ]] || {
			tagarr+=("${tarr[$i]}@1")
#			echo "+ ${tarr[$i]}"
		}
	done

	fontcount=$(sed 's/^.*fontcount="\([^"]*\)".*/\1/' <<<$l)        #"
	typecategory=$(sed 's/^.*typecategory="\([^"]*\)".*/\1/' <<<$l)  #"

	# grab the page with the download link
	href="https://damieng.com$(sed 's/^.*href="\([^"]*\)".*/\1/' <<<$l)"   #'
	hget="${dirh}${href##*/}.html"
	[[ -e "$hget" ]] || wget "$href" -O "$hget"

	# extract the download link - and grab the zip
	file=https://$(grep 'href="[^d]*download' ${hget} | sed 's_^.*href="[^d]*\(download[^"]*\)".*_\1_')  #"
	zip="${file##*/}"
	zipd="${dirh}${zip}"
	htmlfcnt=$(grep -o '<div class="previews">' ${hget} | wc -l)
	[[ -e "$zipd" ]] || wget "$file" -O "$zipd"

	# make a json file
	json="${dir}$(sed 's_\.zip$_\.json_' <<<${zip})" #"

# 'heredoc' outdented!
cat << EOF > "$json"
	{
		"title"        : "$title",
		"tags"         : "$tags",
		"zip"          : "$zip",
		"htmlfcnt"     : "$htmlfcnt",
		"fontcount"    : "$fontcount",
		"font"         : [
EOF
	echo -en "\t$htmlfcnt -> $fontcount"
	(( htmlfcnt != fontcount )) && echo "  ERROR" || echo

	# extract source/*.h
	rm -rf ${dirt}
	mkdir -p ${dirt}

	unzip -qqjo "${zipd}" '*.h' -d "${dirt}"
	comma=0
	# foreach header file
	for i in ${dirt}/*.h ; do
		dos2unix "$i" >/dev/null 2>&1  # convert line endings: M$ -> posix

		((comma)) && (echo "," >> "$json")  # append comma to previous entry

		hdr="${i##*/}"
		font="${hdr%%.h}"

# append to json
echo -en '\t\t\t{  "name"  : "'${font}'" }' >>"$json"

		# pull out the matching png
		png="${font}.png"
		unzip -qqjo "${zipd}" "$png" -d "${dirt}"

		((fcnt++))  # fonts
		comma=1
	done

# the initial blank line is the \n for the last array entry
cat << EOF >> "$json"

		],
		"typecategory" : "$typecategory",
		"source"       : "$href",
		"file"         : "$file",
		"copyright"    : "${title} font by DamienG https://damieng.com/zx-origins"
	}
EOF

	# move the new files in the main folder
#	mv ${dirt}/* ${dir}/
	for i in ${dirt}/* ; do
		fn=${i##*/}
		if [[ -e "${dir}/${fn}" ]] ; then
			echo "    COLLISION: $fn"
		else
			mv "$i" "${dir}/"
		fi
 	done

	((tcnt++))  # typefaces
done < list

#results
echo "# Found: $tcnt typefeaces / $fcnt fonts"
echo "# tags: ${tagarr[*]}"

# cleanup
rm list
rm -rf ${dirt}

##fixup
##echo "# Created: $master"

convert  # .h -> .h|.c

domaster  # zxorigins .c|.h

maketags ${tagarr[@]}

echo -e "\n\n* feel free to erase $dirh\n"
