<?php

/**
  * Abstract Widget Class -- Defines widgety goodness
  *
  * @package anApp
  */

abstract class aWidget {


   /**
     * Outputs success true, for lazy programmers.
     *
     * @return string
     */
    function display() {
        global $debug;
        $debug->level = 0;
        print "{success: true}";
        exit; 
     }  
        


   /**
     * Exports JSON
     * 
     */
    function json($output, $utfSafe = FALSE, $exit = TRUE) {
       global $user, $config;
         
         $output = $this->recursiveUnHtmlize($output);
         if ($utfSafe) { $output = array_map(utf8_encode, $output); }
         $output = json_encode($output);
         $output = preg_replace('/"action:([^\"]+)"/', '$1', $output);
	 if ($config->environment != 'test' && $exit) {
	   header("Content-type: application/json");
	   print $output;
	   exit;
	 } else {
	   return $output;
	 }
     }


   /**
     * Recursively unhtmlize a value
     *
     * @param mixed $value String or array to unhtmlize
     * @param bool $jssafe Javascript safe?
     * @return mixed
     */

     function recursiveUnHtmlize( $value, $jssafe = FALSE ) {
              if ( is_array($value) ) {
                  foreach( $value as $key => $val ) {
                          $value[$this->unhtmlize($key,$jssafe)] = $this->recursiveUnHtmlize($val,$jssafe);
                  }
              } else {
                  $value = $this->unhtmlize($value,$jssafe);
              }
         return $value;
              
     }
    
     
       /**
         * Merge Two Arrays Keeping Keys Unique
         *
         * @param array $array1 First Array
         * @param array $array2 Second Array
         * @param string $prefix Prefix to add to keys of array 2 to keep from overwriting keys in array 1
         * @param bool $alwaysPrefix Always add the prefix to the key.
         * @return array
         */
        function safeMerge( $array1, $array2, $prefix, $alwaysPrefix = FALSE) {
            foreach( $array2 as $key => $val ) {
                if ( array_key_exists( $key, $array1 ) || $alwaysPrefix) {
                    $array1[$prefix.$key] = $val;
                } else {
                    $array1[$key] = $val;
                }
            }
                return $array1;
        }


    function unhtmlize($string, $jssafe = FALSE) {
        if ( preg_match('/&#(0)*39;/', $string) ) {
            $s = preg_replace('/&#(0)*39;/', "'", $string);
            return $s;
        } else {
            return $string;
        }
        
    }

};

?>