<?php
require_once 'PHPUnit/Framework.php';
require_once 'BuildfileRunner.inc';

class buildToolBasicTest extends PHPUnit_Framework_TestCase {
    
    protected $_buildfileXml = null;
    protected $_buildfileName = null;
    protected $_buildfileBasedir = null;
    protected $_buildfileRunner = null;
    protected $_isTearDownNecessary = false;

    protected function setUp() {
        $this->_buildfileName = realpath('build.xml');        
        $this->_buildfileBasedir = dirname($this->_buildfileName);
        $this->_buildfileXml = file_get_contents($this->_buildfileName);
        $this->_buildfileRunner = new BuildFileRunner($this->_buildfileName);
    }
    
   protected function tearDown() {
        if ($this->_isTearDownNecessary) {
            $this->_buildfileRunner->runTarget(array('clean'));
        }
    }
    
    /**
     * @test
     * @group structure
     */
    public function targetMainShouldBeTheDefaultTarget() {
        $xml = new SimpleXMLElement($this->_buildfileXml);
        $xpath = "//@default";
        $defaultElement = $xml->xpath($xpath);
        $this->assertSame('main', trim($defaultElement[0]->default), 
            "Buildfile doesn't have a default target named 'main'");
    }
     /**
     * @test
     * @group structure
    public function propertyGithubReposDirShouldBeSet() {
        $xml = new SimpleXMLElement($this->_buildfileXml);
        $xpath = "//property[@name='github.repos.dir']/@value";
        $valueElement = $xml->xpath($xpath);
        $this->assertTrue($valueElement[0] instanceof SimpleXMLElement, 
            "Buildfile doesn't contain a 'github.repos.dir' property");
        $this->assertGreaterThan(1, strlen($valueElement[0]->value));
    }
    */
 
    /**
     * @test
     * @group structure
     */
    public function buildfileShouldContainACleanTarget() {
        $xml = new SimpleXMLElement($this->_buildfileXml);
        $cleanElement = $xml->xpath("//target[@name='clean']");
        $this->assertTrue(count($cleanElement) > 0, 
            "Buildfile doesn't contain a clean target");
    }
    /**
     * @test
     * @group structure
     */
    public function targetLogBuildShouldBeAPrivateOne() {
        $xml = new SimpleXMLElement($this->_buildfileXml);
        $nameElement = $xml->xpath("//target[@name='-log-build']");
        $this->assertTrue(count($nameElement) > 0, 
            'Log build target is not a private target');
    }
    /**
     * @test
     * @group structure
     */
    public function targetMainShouldDependOnCleanTarget() {
        $xml = new SimpleXMLElement($this->_buildfileXml);
        $xpath = "//target[@name='main']/@depends";
        $dependElement = $xml->xpath($xpath);
        $this->assertTrue(count($dependElement) > 0, 
            'Target build contains no depends attribute');
        $dependentTasks = array_filter(explode(', ', 
            trim($dependElement[0]->depends)
        ));        
        $this->assertContains('clean', $dependentTasks, 
            "Target main doesn't depend on the clean target");
    }
    /**
     * @test
     * @group structure
     */
    public function allDefinedTargetsShouldHaveADescriptionAttribute() {
        $xml = new SimpleXMLElement($this->_buildfileXml);
        $xpath = "//target";
        $targetElements = $xml->xpath($xpath);
        $describedTargetElements = array();
        foreach ($targetElements as $index => $targetElement) {
            $targetDescription = trim($targetElement->attributes()->description); 
            if ($targetDescription !== '') {
                $describedTargetElements[] = $targetDescription;
            }
        }
        $this->assertEquals(count($targetElements), 
            count($describedTargetElements), 
                'Description not for all targets set');
    }
    /**
     * @test 
     * @group artifact
     */
    public function initTargetShouldCreateInitialBuildArtifacts() {
        $this->_isTearDownNecessary = true;
        $this->_buildfileRunner->runTarget(array('init'));

        $expectedInitArtifacts = array(
            "{$this->_buildfileBasedir}/build", 
            "{$this->_buildfileBasedir}/build/logs/performance/", 
            "{$this->_buildfileBasedir}/build/doc",
            "{$this->_buildfileBasedir}/build/reports"
        );

        foreach ($expectedInitArtifacts as $artifact) {
            $this->assertFileExists($artifact, 
                "Expected file '{$artifact}' doesn't exist");
        }
    }
    /**
     * @test
     * @group artifact
     */
    public function sqlFilesForDatabaseSetupTargetShouldBeAvailable() {
        $expectedSqlFiles = array(
            "{$this->_buildfileBasedir}/sqlfiles", 
            "{$this->_buildfileBasedir}/sqlfiles/session-storage.sql", 
            "{$this->_buildfileBasedir}/sqlfiles/acl.sql", 
            "{$this->_buildfileBasedir}/sqlfiles/log.sql"
        );

        foreach ($expectedSqlFiles as $sqlFile) {
            $this->assertFileExists($sqlFile, "SQL file '{$sqlFile}' doesn't exist");
        }
    }
}
?>