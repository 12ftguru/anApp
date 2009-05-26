<?PHP
/**
 * aLib HTML Template class
 * Extraordinarily simple but powerful templates. This is the part
 * where it gets truly truly evil. It doesn't "accept an array." The
 * templates may be called .ihtml, but they're actually PHP
 * code. That's right. Anything PHP can do, our templates can. That's
 * the evil.
 *
 * The philosophy behind template engines is to separate logic from
 * display. In our old implementation, logic included display if it
 * involved code. Now, not so much. You can put logic or display in
 * either place, so it's up to the coder(s) to Do The Right Thing
 * (tm).
 *
 * Now I've been careful with variable scope. None of the variables
 * from outside the template can leak in unexpectedly, or vice versa,
 * but if you call globals from inside the template, it's your own
 * damned fault.
 * @example templates/header.ihtml example container template
 * @example templates/login.ihtml example template with control structures
 * @example demo/userauthtest.php example logic code calling the template object
 * @package aLib
 * @subpackage HTML
 */
class template extends anObject
  {
    private $vars;
    /**
     * Template file to load
     * @var string $file File to load
     */
    public $file;
    /**
     * Constructor.
     * @param string $file Template file to load.
     */
    
    function __construct($file = null) 
      {
          global $user, $debug, $self, $config;
          $this->set('user', $user);
          $this->set('debug', $debug);
          $this->set('self', $self);
          $this->set('config', $config);
          
        $this->file = $file;
        parent::__construct();
      }
    public function __set($name, $value) 
      {
        $this->set($name, $value);
      }
    /**
     * Assign a variable to the template. Is not set within the
     * template until {@link template::parse()} is called.
     * @param string $name name to assign the variable within the template.
     * @param mixed $value variable to pass into the template. If this
     * is another template object, it will be parsed immediately.
     */
    public function set($name, $value) 
      {
        global $debug;
        if (is_object($value) && is_a($value, "template")) 
          {
            //someone passed us a template
            $this->vars[$name] = $value->parse();
            $debug->line("Parsed template into var $name", 10);
            $debug->variable($value, 10);
          }
        else
          {
            $this->vars[$name] = $value;
            $debug->line("Parsed value into var $name", 10);
            $debug->variable($value, 10);
          }
      }
    /**
     * Assign a variable to an array within the template. Is not set
     * within the template until {@link template::parse()} is called.
     * @param string $name name of the array to add the variable to.
     * @param mixed $value variable to pass into the template. If this
     * is another template object, it will be parsed immediately.
     */
    public function push($name, $value) 
      {
        global $debug;
        if (isset($this->vars[$name]) && !is_array($this->vars[$name])) 
          {
            # Convert to an array...
            $temp = array();
            $temp[] = $this->vars[$name];
            $this->vars[$name] = $temp;
          }
        if (is_object($value) && get_class($value) == 'template') 
          {
            //someone passed us a template
            $this->vars[$name][] = $value->parse();
            $debug->line("Parsed template into var $name", 10);
            $debug->variable($value, 10);
          }
        else
          {
            $this->vars[$name][] = $value;
            $debug->line("Parsed value into var $name", 10);
            $debug->variable($value, 10);
          }
      }
    /**
     * Parse the template and return the results as a string.
     * @param string $file [optional] the name of the template
     * file. Yes, you can change template files midstream.
     * @return string HTML
     */
    public function parse($file = null) 
      {
        global $debug;
        $debug->debug('Parsing from: %s', __CLASS__, 3);
        
        if (!$file) $file = $this->file;
        if (is_array($this->vars)) 
          {
            extract($this->vars);
          }
        else
          {
            $debug->line("template.inc: No variables to interpolate", 1);
          }
        $debug->line('including file: ' . $file, 11);
        if ($file != '') 
          {
            ob_start();
            include ($file);
            $contents = ob_get_contents();
            ob_end_clean();
          }
        $debug->line('returning contents: <br />' . $contents, 11);
        return $contents;
      }
    /**
     * Same as parse but goes ahead and outputs the HTML
     * @param string $file [optional] the name of the template
     * file. Yes, you can change template files midstream.
     */
    public function display($file = null) 
      {
        $output = $this->parse($file);
        print $output;
      }

    public function clear($file = null) 
      {
          global $user, $debug, $self, $config;
          $this->vars = array();
          $this->set('user', $user);
          $this->set('debug', $debug);
          $this->set('self', $self);
          $this->set('config', $config);
      }


    
  }
?>