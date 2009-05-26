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

define('DEBUGFIREBUG', -2);


class Debug
  {
    /**
     * The level of our debuggery.
     * zero means none. Higher numbers mean more verbose output. Most aLib
     * objects start crying for mama at debug levels of greater than ten.
     * @var int $level 0-whatever
     */
    public $level; # Our DebugLevel
    
    /**
     * Output type
     * @var string $output may be either 'plain', 'pretty' or 'template'. Default is pretty.
     * @see Debug::dump()
     */
    public $output; # may be "plain" or "pretty"
    public $_debugData;
    private static $one_ping_only;
    /**
     * Never use this.
     */
    private 
    function __construct($debuglevel = 0, $output = "pretty") 
      {
        ob_start();
        $this->_debugData = array();
        $this->level = $debuglevel;
	include_once('FirePHP.class.php');
	$this->firephp = FirePHP::getInstance(true);
	$this->line("Debug Object Created", 1);
      }
    /**
     * Don't ever use this. I <i>will</i> hurt you.
     */
    public static 
    function singleton() 
      {
        if (!isset(self::$one_ping_only)) 
          {
            $c = __CLASS__;
            self::$one_ping_only = new $c;
          }
        return self::$one_ping_only;
      }
/**
  * Shows included files.
  *
  * @param int $level Debug Level 
  */
    public function showFiles( $level ) {
      $this->debug('Included Files: %s', get_included_files(), $level);      
    }

    
    /**
     * Our main debugging function. Works like sprintf except complex variables are var_exported.
     * The last argument is always the debug level, and is not displayed
     * in the debugging info.
     *
     * @return none
     */
    public 
    function debug() 
      {
        $numArgs = func_num_args();
        $args = func_get_args();
        $printArgs = array();
        $level = array_pop($args);
	  
        if ($level <= $this->level) 
          {

            if ($numArgs > 2) 
              {
                $format = array_shift($args);
                $format = preg_replace('/\%s/', '<span class="debugstring">%s</span>', $format);
                
                foreach($args as $arg) 
                  {
                    if (is_object($arg) || is_array($arg) || is_resource($arg)) 
                      {
                        ob_start();
                        if ($this->output != "plain") 
                          {
                            var_export($arg);
                            $string = ob_get_contents();
                            ob_end_clean();
                            $string2 = highlight_string('<?' . $string . '?>', TRUE);
                            $string = "<b>[</b> <pre class=\"debugPre\">" . str_replace("&nbsp;", " ", $string2) . "</pre> <b>]</b>";
                          }
                        else
                          {
                            var_export($arg);
                            $string = ob_get_contents();
                            ob_end_clean();
                          }
                        $printArgs[] = $string;
                      }
                    else
                      {
                        $printArgs[] = $arg;
                      }
                  }
                $output = vsprintf($format, $printArgs);
              }
            else
              {
                $output = $args[0];
              }
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

	      	if ($this->output == 'firebug') {
  $fileA = preg_split('#[/\\\]#', $bt[$btLine]['file']);
  $file = $fileA[count($fileA) - 1];
	   foreach ($args as $arg) {
	         $this->firephp->fb($arg ,$level." F:".$file." L:".$bt[$btLine]['line'], FirePHP::LOG);
	   }
	    
	    
	} else {

            $this->_debugData[] = array(
                'time' => $this->_getmicrotime() ,
                'string' => $output,
                'level' => $level,
                'mem' => $mem,
                'type' => 'log',
                'line' => $bt[$btLine]['line'],
                'file' => $bt[$btLine]['file']
            );
  }
            $bt = array();
          }
      
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
        $this->debug("Variable: %s", $mixed, $level);
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


   /**
     * Save the info from an exception
     *
     * @param Exception $exception 
     * @return none
     */
    function exception( $exception ) {
            $this->_debugData[] = array(
                'time' => $this->_getmicrotime() ,
                'exception' => $exception,
                'level' => 1,
                'type' => 'exception'
            );
     }
    
    /**
     * The way to log a backtrace. This is similar to debug_backtrace, but better.
     * @param int $level The debug level at which this string should be logged.
     */
    
    function backtrace($level) 
      {
        if ($level <= $this->level) 
          {
            $bt = debug_backtrace();
            $this->_debugData[] = array(
                'time' => $this->_getmicrotime() ,
                'bt' => $bt,
                'level' => $level,
                'type' => 'bt'
            );
          }
      }
    
    function _getmicrotime() 
      {
        list($usec, $sec) = explode(" ", microtime());
        return ((float)$usec+(float)$sec);
      }
    /**
     * If we've logged stuff and the debug level is greater than zero,
     * we'll spew debug info when the object is destroyed.
     */
    
    function __destruct() 
      {
        $this->dump();
      }
    /**
     * This will output the debug info based on {@link Debug::output}.
     * Running this will finish your debugging for the session. Debug
     * level will be set to zero. Really, only call this when you're
     * done debugging.
     * @param int $return [optional] if non-zero, will return html. If
     * zero, will print the results without returning html.
     * @return string $html [optional] html
     */
    
    function dump($return = 0) 
      {
        if ($this->level > 0 && $return == 0) 
          {
            if ($this->output == "plain") 
              {
                $this->dump_plain();
              }
            elseif($this->output == "pretty") 
              {
                $this->dump_pretty();
              }
            elseif($this->output == "template") 
              {
                $this->dump_template();
              }
            elseif ($this->output == "firebug") {
                 return;
           }
            else
              {
                $this->dump_pretty();
              }
          }
        elseif($this->level > 0) 
          {
            ob_start();
            $this->dump(0);
            $contents = ob_get_contents();
            ob_end_clean();
            return ($contents);
          }
        $this->level = 0;
      }
    protected 
    function dump_template() 
      {
        // Note! If your debug display is template,
        // do not display the template yourself, since
        // the debug dump comes after program execution.
        $template = new template($this->template);
        $string = ob_get_contents();
        ob_end_clean();
        $output = preg_replace('/<\/body>\s*<\/html>/', '', $string);
        $template->set('data', $output);
        $template->set('debugLevel', $this->level);
        $template->set('debugData', $this->_debugData);
        $template->display();
      }
    protected 
    function dump_plain() 
      {
        $string = ob_get_contents();
        ob_end_clean();
        print $string . "\n";
        print "Time\t\tDebug Level\t\tData\n";
        foreach($this->_debugData as $datum) 
          {
            print $datum['time'] . "\t\t" . $datum['level'] . "\t\t" . $datum['string'] . "\n";
          }
        print $this->_getmicrotime() . "\t\t1\t\tDebug Info Printed.\n";
      }
    protected 
    function dump_pretty() 
      {
        $string = ob_get_contents();
        ob_end_clean();
        print $string . "\n";
        $row = 1;
        print "<div id=\"debugarea\"><table id=\"debugtable\" class=\"debugtable\" summary=\"Debug Output\">\n";
        print "<caption>Debug Output</caption>\n";
        print "<thead>\n";
        print "<tr class=\"debugheader\">\n";
        print "<th class=\"debugheader\">Time</th>\n<th class=\"debugheader\">Elapsed</th>\n<th class=\"debugheader\">Memory</th>\n<th class=\"debugheader\">Debug Level</th>\n<th>File</th><th>Line</th><th class=\"debugheader\">Data</th>";
        print "</tr>\n</thead>\n<tbody>";
        /*    $datum = array_shift($this->_debugData);
        print "<tr class=\"debugdataodd\">\n";
        print "<td class=\"debugtime\">" . date('j-m-y G:i:s T', $datum['time']) . "</td>\n";
        print "<td class=\"debugelapsed\"> -- </td>\n";
        print "<td class=\"debuglevel\">" . $datum['level'] . "</td>\n";
        print "<td class=\"debugfile\">" . $datum['file'] . "</td>\n";
        print "<td class=\"debugline\">" . $datum['line'] . "</td>\n";
        print "<td class=\"debugdata\">" . $datum['string'] . "</td>\n";
        print "</tr>\n";*/
        $last = 0;
        $start = $last;
        foreach($this->_debugData as $datum) 
          {
            $elapsed = $datum['time']-$last;
            if ($start == 0) 
              {
                  $elapsed = 0;
                  
                $start = $datum['time'];
              }
            $row++;
            $class = ($row%2 == 0) ? "even" : "odd";
            print "<tr class=\"debugdata" . $class . "\">\n";
            print "<td class=\"debugtime\">" . date('G:i:s j-m-y', $datum['time']) . "</td>\n";
            print "<td class=\"debugelapsed\">" . $elapsed . "</td>\n";
            print "<td class=\"debugmemory\">" . $datum['mem'] . "</td>\n";
            print "<td class=\"debuglevel\">" . $datum['level'] . "</td>\n";
            print "<td class=\"debugfile\">" . $datum['file'] . "</td>\n";
            print "<td class=\"debugline\">" . $datum['line'] . "</td>\n";
            if ($datum['type'] == 'log') 
              {
                print "<td class=\"debugdata\">" . $datum['string'] . "</td>\n";
              }
            elseif($datum['type'] == 'var') 
              {
                print "<td class=\"debugvar\"><pre>" . $this->_colorSource($datum['string']) . "</pre></td>\n";
              }
            elseif($datum['type'] == 'bt') 
              {
                print "<td class=\"debugvar\"><pre>";
                var_dump($datum['bt']);
                print "</pre></td>\n";
              }
            elseif($datum['type'] == "exception") 
              {
                $e = $datum['exception'];
                
                print "<td class=\"debugvar\">";
                
                $htmldoc = "An exception was thrown :<br/>";
                $htmldoc.= "Exception code : $e->getCode()<br/>";
                $htmldoc.= "Exception message : $e->getMessage()<br/>";
                $htmldoc.= "Stack Trace: <b>[</b><pre class=\"debugPre\">";
		    $htmldoc .= $e->getTraceAsString();
                $htmldoc.= "<b>]</b></pre>";
                print $htmldoc;
                print "</td>\n";
              }
            print "</tr>\n";
            $last = $datum['time'];
          }
        $elapsed = $last-$start;
        print "</tbody>\n<tfoot><tr class=\"debugsummary\"><td colspan=4>Total elapsed time: " . $elapsed . "</td></tr></tfoot>\n";
        print "</table></div>\n";
      }
    public function dump_anAppJSON() 
      {

        $retval = array();

          if ( $this->level > 0 ) {
        $string = ob_get_contents();
        ob_end_clean();
        $last = 0;
        $start = $last;
        foreach($this->_debugData as $datum) 
          {
              $dataRow = array();
              
            $elapsed = $datum['time']-$last;
            if ($start == 0) 
              {
                  $elapsed = 0;
                  
                $start = $datum['time'];
              }
            $row++;
            $dataRow['tcol-0'] = date('G:i:s j-m-y', $datum['time']);
            $dataRow['tcol-1'] = $elapsed;
            $dataRow['tcol-2'] = $datum['mem'];
            $dataRow['tcol-3'] = $datum['level'];
            $dataRow['tcol-4'] = $datum['file'];
            $dataRow['tcol-5'] = $datum['line'];
            if ($datum['type'] == 'log') 
              {
                $dataRow['tcol-6'] = $datum['string'];
            }
            elseif($datum['type'] == 'var') 
              {
                  $dataRow['tcol-6'] = "<pre>" . $this->_colorSource($datum['string']) . "</pre>";
              }
            elseif($datum['type'] == 'bt') 
              {
                  ob_start();
                  var_export($datum['bt']);
                  $string = ob_get_contents();
                  ob_end_clean();
                  $string2 = highlight_string('<? ' . $string . ' ?>', TRUE);
                  $colorsource = str_replace("&nbsp;", ' ', $string2);
                  $dataRow['tcol-6'] = "<pre>".$colorsource."</pre>";
              }
            elseif($datum['type'] == "exception") 
              {
                $e = $datum['exception'];
                
                $dataRow['tcol-6'] = "<h3 class=\"debugException\">An exception was thrown :</h3><br/>";
                $dataRow['tcol-6'].= "Exception type : ".get_class($e)."<br/>";
                $dataRow['tcol-6'].= "Exception code : ".$e->getCode()."<br/>";
                $dataRow['tcol-6'].= "Exception message : ".$e->getMessage()."<br/>";
                $dataRow['tcol-6'].= "Stack Trace: <span class=\"debugstring\"><b>[</b><pre class=\"debugPre\">";
                 ob_start();
                  var_export($e->getTrace());
                  $string = ob_get_contents();
                  ob_end_clean();
                  $string2 = highlight_string('<? ' . $string . ' ?>', TRUE);
                  $colorsource = str_replace("&nbsp;", ' ', $string2);
                  $dataRow['tcol-6'] .= $colorsource;
                $dataRow['tcol-6'].= "</pre><b>]</b></span>";

              }
          $retval[] = $dataRow;
          $last = $datum['time'];
          }
        }
        return $retval;
        
      }
    
    function _colorSource($source) 
      {
        $colorsource = highlight_string('<?' . $source . '?>', TRUE);
        $string2 = str_replace("&nbsp;", " ", $colorsource);
        $colorsource = str_replace(array(
            "<code><span style=\"color: #000000\">\n<span style=\"color: #0000BB\">&lt;?</span>",
            "<span style=\"color: #0000BB\">?&gt;/**
* does Pre but returns a string
*
*/</span>\n</span>\n</code>"
        ) , array(
            '<code>',
            '</code>'
        ) , $string2);
        return $colorsource;
      }
  }
/**
 * Pretty prints a mixed variable inline. Prints nothing if debug level is 0. Does print if debug level is -1 (so you can have pre on but debugging info off.)
 *
 * @param mixed $mixed Any variable.
 * @return none
 */


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
