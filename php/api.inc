<?PHP


/**
 * anApp API -- Based on the Ext.Direct API and the php router by Tommy Maintz
 *              http://www.extjs.com/forum/showthread.php?t=68186
 *
 *
 * @category API
 * @package anApp
 * @subpackage API
 * @author Bryan P Johnson <bryan@12ftguru.com>
 * @license http://www.opensource.org/licenses/bsd-license.php BSD
 * @version 1.0
 * @link 
 */
class api {
   public function __construct() {
     global $config;
     addIncludePath('api', TRUE);
     require_once('ExtDirect/API.php');
     require_once('ExtDirect/Router.php');
     require_once('ExtDirect/CacheProvider.php');
     // this should alwasy be set but if its not, then execute api.php without outputting it

     $cache = new ExtDirect_CacheProvider('api/cache/api_cache.txt');
     $this->api = new ExtDirect_API();


     $this->api->setRouterUrl($config->apiURL); // default
     $this->api->setCacheProvider($cache);
     $this->api->setNamespace($config->apiName);
     $this->api->setDescriptor($config->apiDescriptor);
     $this->api->setDefaults(array(
			     'autoInclude' => true,
			     'basePath' => '.'
			     ));

     $this->api->add(
		     $config->apiMethods
		     );

     $this->handleAPICall();
     exit;
   }

   function outputAPI() {
     global $config;

     setcookie($config->appName.'-api-state', true);

     $this->api->output();
     

   }

   function handleAPICall() {
     //global $fakeData;
     //     $fakeData = '{"action":"Files","method":"getPhotosByProperty","data":[1],"type":"rpc","tid":1}';
     $router = new ExtDirect_Router($this->api);
     $router->dispatch();
     $router->getResponse(true); // true to print the response instantly
   }

}

?>