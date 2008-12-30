<?php
/**
 * Loads an alib configuration file into local scope for later handling.
 *
 *
 * @author   BrYan P. Johnson
 * @package aLib
 * @version 1.0
 */
/**
 * Our config class.
 *
 * @package aLib
 */
class config extends anObject
  {
    private $_configVars;
    public $loadedFiles;
    /**
     * Create our config object
     *
     * @param string $filename Config file to load
     * @return object
     */
    public 
    function __construct($filename = FALSE) 
      {
        if ($filename) 
          {
            $this->loadFile($filename);
          }
      }
    public 
    function __get($name) 
      {
        global $debug;
        if ($debug->level >= 10) 
          {
            $debug->line("Object (" . get_class($this) . ") __get $name = ", 10);
            $debug->variable($this->_configVars[$name], 10);
          }
        return $this->_configVars[$name];
      }
    public 
    function __set($name, $value) 
      {
        return false;
      }
    /**
     * Load an additional config file
     *
     * @param string $filename File to load
     * @return bool
     */
    public 
    function loadFile($filename) 
      {
        if ($filename) 
          {
            global $debug;
            $origVars = get_defined_vars();
            $origVars['origVars'] = TRUE;
            if (include_once ($filename)) 
              {
                if ($debug->level >= 10) 
                  {
                    $debug->line('Loading config file: ' . $filename, 10);
                  }
                $this->loadedFiles[] = $filename;
                $newVars = get_defined_vars();
                $loadedVars = array_diff(array_keys($newVars) , array_keys($origVars));
                foreach($loadedVars as $key) 
                  {
                    if ($debug->level >= 10) 
                      {
                        $debug->line('Setting config variable: ' . $key, 10);
                      }
                    $this->_configVars[$key] = $newVars[$key];
                  }
              }
          }
      }
  };
global $config, $configFile;
if ($configFile) 
  {
    $config = new config($configFile);
  }
else
  {
    $config = new config();
  }
?>
