<?
/**
 * Utility class for testing Phing Buildfiles
 *
 * Based on code from http://raphaelstolt.blogspot.com/2009/05/testing-phing-buildfiles-with-phpunit.html
 *
 * @package anApp
 * @subpackage buildTool
 * @version 1.0
 * @author Bryan P. Johnson <bryan@12ftguru.com>
 */


class BuildFileRunner {

    private $_buildfilePath = null;
    public $phingCommand = "phing";

    public function __construct($buildfilePath) {
        if (!file_exists($buildfilePath)) {
            throw new Exception("Buildfile '{$buildfilePath}' doesn't exist");
        }
        $this->buildfilePath = realpath($buildfilePath);
    }
    public function runTarget($targets = array(), $properties = array()) {
        $runTargetCommand = $this->phingCommand . " "
                          . "-f {$this->buildfilePath} ";
        if (count($targets) > 0) {
            foreach ($targets as $target) {
                $runTargetCommand.= $target . " ";
            }
        }
        if (count($properties) > 0) {
            foreach ($properties as $property => $value) {
                $runTargetCommand.= "-D{$property}={$value} ";
            }       
        }
        exec(escapeshellcmd($runTargetCommand), $output, $return);
        return array('output' => $output, 'return' => $return);
    }
}




?>