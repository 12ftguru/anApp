<?php
/**
 * The Debug Object and then some.
 *
 * There is only ever one debug object. You will never create
 * it. When you first include alib.inc, the debug object is created
 * and placed both in $AGLOBAL['DEBUG'] and the global variable
 * $debug.
 *
 * Note that the {@link $AGLOBAL} variable stores a reference to the
 * debug object in $MGLOBAL['DEBUG']
 * @package aLib
 * @subpackage Debuggery
 */

include_once('FirePHP.class.php');


class Debug extends FirePHP {
  protected static $instance = null;
  protected static $myTable = array();

  
  function __construct() {
    ob_start();
    parent::__construct();
    $this->registerErrorHandler();
    $this->registerExceptionHandler();
    $options = array('includeLineNumbers' => true);
    $this->setOptions($options);
    $this->myTable[] = array('Time', 'Level', 'File', 'Line', 'Memory', 'Message');
  }


  public static function singleton () {
    return Debug::getInstance(TRUE);
  }

  public static function getInstance($AutoCreate=false) {
    if($AutoCreate===true && !Debug::$instance) {
      Debug::init();
    }
    return Debug::$instance;
  }


    public static function init() {
    return Debug::$instance = new Debug();
  }


    /**
     * For backwards compatibility...
     *
     * @param mixed $mixed The variable to log.
     * @param int $level The debug level
     * @return none
     */
    
    function variable($mixed, $level) 
      {
        $this->debug($mixed, $level);
      }
    /**
     * For backwards compatibility...
     *
     * @param string $string The string to log.
     * @param int $level The debug level
     * @return none
     */
    
    function line($string, $level) 
      {
	  $this->debug($string, $level);
      }

    function _getmicrotime() 
      {
        list($usec, $sec) = explode(" ", microtime());
        return ((float)$usec+(float)$sec);
      }

  public function debug() {
    $numArgs = func_num_args();
    $args = func_get_args();
    $level = array_pop($args);
    if ($level <= $this->level) 
      {
	$bt = debug_backtrace();
	if (strstr($bt[0]['file'], 'debug.inc')) 
	  {
	    $btLine = 1;
	  }
	else
	  {
	    $btLine = 0;
	  }
	if (function_exists('memory_get_usage')) 
	  {
	    $mem = memory_get_usage(TRUE);
	  }
	else
	  {
	    $mem = '???';
	  }
#	$now = $this->_getmicrotime();
	$now = time();
	  foreach($args as $arg) {
	    $this->myTable[] = array($now, $level, $bt[$btLine]['file'], $bt[$btLine]['line'], $mem, $arg);
	  }

      }
  }

  function __destruct() {
    if (count($this->myTable) > 1) {
      $this->table('aLib/anApp Debug Info ['.count($this->myTable).' item(s)]', $this->myTable);
    }
        $string = ob_get_contents();
        ob_end_clean();
        print $string;

  }

  public function showFiles( $level ) {
      $this->debug('Included Files:', get_included_files(), $level);      
  }

}

function preString( $mixed ) {
    ob_start();
    pre($mixed);
    $string = ob_get_contents();
    ob_end_clean();
    return $string;
}

function pre($mixed) 
  {
    global $debug;
    if ($debug->level != 0) 
      {
        print ("<pre>");
        if (is_string($mixed)) 
          {
            print ($mixed);
          }
        else
          {
            ob_start();
            var_export($mixed);
            $string = ob_get_contents();
            ob_end_clean();
            $string2 = highlight_string('<? ' . $string . ' ?>', TRUE);
            $colorsource = str_replace("&nbsp;", ' ', $string2);
            print ($colorsource);
          }
        print ("</pre>");
      }
  }








?>
