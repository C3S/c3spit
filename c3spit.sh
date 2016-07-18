#  Copyright 2016, Cultural Commons Collecting Society SCE mit
#                       beschränkter Haftung (C3S SCE)
#  Copyright 2016, Thomas Mielke <thomas.mielke@c3s.cc>
#  
#  c3spit is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.  
#  c3spit is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.  
#  You should have received a copy of the GNU General Public License
#  along with c3spit.  If not, see <http://www.gnu.org/licenses/>.   

while [ 1 ]; do
	echo "recording ..."
	./c3spit hw:0 >> log.txt
	echo "creating fingerprint ..."
	./echoprint-codegen/echoprint-codegen samples.wav > postdata
	echo "sending fingerprint to the server ..."
	jq -r '.[0].code' < postdata > code
	c=`cat code`
	curl https://echoprint.c3s.cc/query?fp_code=$c > result
	jq -r '.message' < result > message
	m=`cat message`
	echo "$m"
	if [[ $m != "no results found (type 7)" ]]; then 
		old_tid=`cat track_id`
		jq -r '.track_id' < result > track_id
		tid=`cat track_id`
		if [[ $tid != $old_tid ]]; then 
			jq -r '.artist' < result > artist
			jq -r '.track' < result > track
			a=`cat artist`
			t=`cat track`
			if [[ $a != "null" ]]; then 
				echo "$a - $t"
				say $a > /dev/null 2> /dev/null
				say $t > /dev/null 2> /dev/null
			fi
			echo ">>> recognized $a - $t <<<" >> log.txt
		else
			echo "still playing $a - $t ..."
		fi
	fi
done

