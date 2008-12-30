<?
function htmlutf8entities ($string) {
    #curly quotes
    global $debug;
    $debug->debug("utf8 %s", $string, 1);
    $search = array(chr(145), 
                    chr(146), 
                    chr(147), 
                    chr(148), 
                    chr(151)); 
 
    $replace = array("'", 
                     "'", 
                     '"', 
                     '"', 
                     '-'); 
 
    #$string = str_replace($search, $replace, $string); 
    
    $debug->debug("utf8 %s", $string, 1);


    return htmlentities($string, ENT_QUOTES, 'UTF-8');

}


function unhtmlize($string, $jssafe = FALSE) {

    $search = array(chr(145), 
                    chr(146), 
                    chr(147), 
                    chr(148), 
                    chr(151),
                    chr( 226 )
                    ); 
 
    $replace = array("'", 
                     "'", 
                     '"', 
                     '"', 
                     '-',
                     '-'); 
 
    $string = str_replace($search, $replace, $string);

    $s = html_entity_decode( $string, ENT_QUOTES, 'UTF-8' );
    
    $s = preg_replace('/&#(0)*39;/', "'", $s);
    if ($jssafe) {
        $s = preg_replace('/\r\n/',"\\n",$s);
        $s = preg_replace('/\'/', "\'", $s);
    }

    return $s;
}

function truncate($string, $limit = 50, $singleLine=TRUE) {
# simple function to truncate a string to a certain word limit
  $string2 = preg_replace('/\<(br|p)[ \/]*\>/', '\n', $string);
  $string2 = preg_replace('/\&(amp\;)*nbsp\;/', ' ', $string2);
  
    if ( $singleLine ) {
        $temp = explode( '\n', $string2 );
     $string2 = $temp[0];
    }
	$string = strip_tags($string2);
	$fullArray = explode(" ", $string);
	$array = explode(" ", $string, $limit+1);
#	print count($array)." ".count($fullArray)." ".$limit;
	if (count($fullArray) > $limit) {
	# print "yes";
		$append = "...";
	}
	if(count($array) > $limit) {
		array_pop($array);
	}
	#	pre($array);
	$string2 = implode(" ", $array) . $append;
			  $string = preg_replace('/\\\\n/', '<br />', $string2);

	return $string;
}

function splitURL($string, $chunkLen = 50) {
# simple function used with unhtmlize to truncate a url to a certain limit for display
	$chunked_str = chunk_split($string, $chunkLen);
	$output = str_replace( "\r\n", "&lt;br&gt;", $chunked_str );
	return $output;
}

function multiline($text) {
  $retval = preg_replace("/\n/", "<br />\n", $text);
  return $retval;
}

function formatPhone($number) {
  $number = preg_replace("/[^0-9]/","",$number);
  $npa = substr($number, 0,3);
  $nxx = substr($number, 3,3);
  if (strlen($number) == 10) {
  $rest = substr($number, 6);
  $string = "$npa-$nxx-$rest";
  } else {
    $rest = substr($number,6,4);
    $ext = substr($number,10);
    $string = "$npa-$nxx-$rest x$ext";
  }
  return $string;

}

function formatMoney($text) {
  $retval = "$".number_format($text,2);
  return $retval;
}

function formatSocial($text) {
  $temp = preg_replace("/[^0-9]*/", "", $text);
  $temp = str_pad($temp, 9, "0", STR_PAD_LEFT);
  $one = substr($temp, 0, 3);
  $two = substr($temp, 3, 2);
  $three = substr($temp, 5);
  $retval = $one ."-". $two ."-". $three;
  return $retval;

}


?>
