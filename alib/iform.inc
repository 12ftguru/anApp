<?php
/**
 * Improved Form Handling
 * More secure, granular form handling. Includes form filtering and validation.
 *
 * Created: 2007-10-31
 * Last update: 2008-04-25
 *
 * @author   BrYan P. Johnson
 * @package aLib
 * @version 1.0
 */
/* Our Error Levels */
/** 
 * @constant Requires Filtered Variables and communication via SSL
 */
define("PARANOIDFORM", 3);
/** 
 * @constant Requires that variables be Filtered and explicitly allowed.
 */
define("STRICTFORM", 2);
/** 
 * @constant Strips Slashes and HTML Encodes all Variables
 */
define("MODERATEFORM", 1);
/** 
 * @constant Allow all form submissions, do nothing.
 */
define("LAZYFORM", 0);
/**
 * aLib iForm (improved form) class
 *
 * @link http://phpro.org/tutorials/Filtering-Data-with-PHP.html
 * @package aLib
 * @subpackage iForm
 */
class iForm extends anObject
  {
    private $iFormVars;
    public $safetyLevel;
    public $size;
    
    /**
     * Create the iForm Object
     *
     * @param int $safetyLevel Sets the safety level of the form. See constants for more.
     * @param bool $addAll TRUE to add $_POST, $_GET and $_COOKIE vars to the form object by default.
     * @return object
     */
    public 
    function __construct($safetyLevel = 0, $addAll = FALSE) 
      {
        $this->safetyLevel = $safetyLevel;
        parent::__construct();
        if ($addAll) 
          {
            $this->addAll();
          }
      }
    /**
     * Override the default GET function
     *
     */
    
    function __get($name) 
      {
      	if ($name == 'filterHTML') {
      	return new iFormFilterHTML($this->iFormVars);
      	} else if ($name != 'filter') 
          {
            if (isset($this->iFormVars[$name])) 
              {
                $iFormVar = $this->iFormVars[$name];
                if ($this->safetyLevel == 0) 
                  {
                    return ($iFormVar->isClean) ? $iFormVar->getValue() : $iFormVar->getValue(TRUE);
                  }
                elseif($this->safetyLevel == 1) 
                  {
                    $iFormVar->filter(FILTER_SANITIZE_SPECIAL_CHARS, FILTER_FLAG_ENCODE_HIGH);
                    return $iFormVar->getValue();
                  }
                elseif($this->safetyLevel == 3) 
                  {
                    global $_SERVER;
                    if ($_SERVER['HTTPS'] == 'on') 
                      {
                        return $iFormVar->getValue();
                      }
                  }
                else
                  {
                    return $iFormVar->getValue();
                  }
              } else {
                  return FALSE;
              }
          }
        else
          {
            return new iFormFilter($this->iFormVars);
          }
      }
    /**
     * Add $_POST, $_GET and $_COOKIE to the form object.
     *
     * @return none
     */
    
    function addAll() 
      {
        $this->addGET();
        $this->addPOST();
        $this->addCOOKIE();
      }
    /**
     * Add $_POST to the form object.
     *
     * @return none
     */
    
    function addPOST() 
      {
        foreach($_POST as $name => $value) 
          {
            $this->iFormVars[$name] = new iFormVar($value);
          }
        $this->size = count($this->iFormVars);
        
      }
    /**
     * Add $_COOKIE to the form object.
     *
     * @return none
     */
    
    function addCookie() 
      {
        foreach($_COOKIE as $name => $value) 
          {
            $this->iFormVars[$name] = new iFormVar($value);
          }
        $this->size = count($this->iFormVars);
      }
    /**
     * Add $_GET to the form object.
     *
     * @return none
     */
    
    function addGET() 
      {
        foreach($_GET as $name => $value) 
          {
            $this->iFormVars[$name] = new iFormVar($value);
          }
        $this->size = count($this->iFormVars);
      }
    /**
     * Add many filters to the form.
     *
     * @param mixed $filters Associative array of 'varName' => array('filter' => SOME_FILTER, 'options' => SOME_FLAGS)
     * @return none
     */
    
    function addFilters($filters) 
      {
        foreach($filters as $varName => $optionsArray) 
          {
            if (isset($this->iFormVars[$varName])) 
              {
                if (isset($optionsArray['flags'])) 
                  {
                    $this->iFormVars[$varName]->filter($optionsArray['filter'], $optionsArray['flags']);
                  }
                else
                  {
                    $this->iFormVars[$varName]->filter($optionsArray['filter']);
                  }
              }
          }
      }
  };
/**
 * aLib iForm form variable
 * Supports validation and filtering
 *
 * @package aLib
 * @subpackage iForm
 */
class iFormVar extends anObject
  {
    private $rawVar;
    private $lastFilterType;
    private $lastFilterOptions;
    public $filteredValue;
    public $isClean;
    /**
     * Create the iFormVar object
     *
     * @param mixed $value <description>
     * @return object
     */
    
    function __construct($value) 
      {
        $this->rawVar = $this->removeSlashes($value);
        $this->isClean = FALSE;
      }
    /**
     * Filter the variable
     *
     * @param constant $type Type of filter to apply
     * @param array $options Associative array of options or bitwise disjunction of flags. If filter accepts options, flags can be provided in "flags" field of array. For the "callback" filter, callback type should be passed.
     * @return filtered value
     */
    
    function filter($type, $options = array()) 
      {
        if (count($options) > 0) 
          {
            $this->filteredValue = filter_var($this->rawVar, $type, $options);
          }
        else
          {
            $this->filteredValue = filter_var($this->rawVar, $type);
          }
        $this->isClean = TRUE;
        $this->lastFilterType = $type;
        $this->lastFilterOptions = $options;
        return $this->filteredValue;
      }
    private 
    function removeSlashes($input) 
      {
        if (!get_magic_quotes_gpc() || (!is_string($input) && !is_array($input))) 
          {
            return $input;
          }
        if (is_string($input)) 
          {
            $output = stripslashes($input);
          }
        elseif(is_array($input)) 
          {
            $output = array();
            foreach($input as $key => $val) 
              {
                $newval = $this->removeSlashes($val);
                $output[$key] = $newval;
              }
          }
        return $output;
      }
    /**
     * Returns the sanitized value of the variable, unless allowDirty is true, whereupon it returns either the sanitized value (if set) or the dirty value.
     *
     * @param bool $allowDirty Allow the object to return the dirty variable.
     * @return mixed
     */
    
    function getValue($allowDirty = FALSE) 
      {
        if ($allowDirty) 
          {
            return ($this->filteredValue) ? $this->filteredValue : $this->rawVar;
          }
        else
          {
            return $this->filteredValue;
          }
      }
    
    function __toString() 
      {
        return $this->getValue();
      }
  };
/**
 * proxy for filtering. Do not use directly.
 *
 * @package aLib
 * @subpackage iForm
 */
class iFormFilter extends anObject
  {
    private $formVars;
    /**
     * create the object
     *
     * @param ignore $formVars <description>
     */
    
    function __construct($formVars) 
      {
        $this->formVars = $formVars;
      }
    /**
     * override the call var
     *
     * @param function name $func <description>
     * @param $array $args <description>
     */
    
    function __call($func, $args) 
      {
        /*        pre($this); */
        if (isset($this->formVars[$func])) 
          {
            return call_user_func_array(array(
                $this->formVars[$func],
                'filter'
            ) , $args);
          }
        else
          {
            return FALSE;
          }
      }
  };
  
  /**
 * proxy for filtering html. Do not use directly.
 *
 * @package aLib
 * @subpackage iForm
 */
class iFormFilterHTML extends anObject
  {
    private $formVars;
    /**
     * create the object
     *
     * @param ignore $formVars <description>
     */
    
    function __construct($formVars) 
      {
        $this->formVars = $formVars;
      }
    /**
     * override the call var
     *
     * @param function name $func <description>
     * @param $array $args <description>
     */
    
    function __call($func, $args) 
      {
        /*        pre($this); */
        if (isset($this->formVars[$func])) 
          {
            return htmlentities(call_user_func_array(array(
                $this->formVars[$func],
                'filter'
            ) , $args));
          }
        else
          {
            return FALSE;
          }
      }
  };
?>