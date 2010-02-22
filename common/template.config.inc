<?php

$environment = "@environment@"; //dev or test or prod

if ($environment == "dev") {

  include_once('dev.dbinfo.inc');
  include_once('dev.pathinfo.inc');

  $debugConfig = array(
                       'groupByClass' => FALSE, //if output handles grouping, group logs by classname.
                       'debugLevel' => DEBUG_LOW | DEBUG_TRACE,
                       'handleErrors' => TRUE,
                       'showIncludedFiles' => TRUE,
                       'detailedOutput' => TRUE
                       );


} else if ($environment == "test") {

  include_once('test.dbinfo.inc');
  include_once('test.pathinfo.inc');

  $debugConfig = array(
                       'groupByClass' => FALSE, //if output handles grouping, group logs by classname.
                       'debugLevel' => DEBUG_OFF,
                       'handleErrors' => FALSE,
                       'showIncludedFiles' => FALSE,
                       'detailedOutput' => FALSE
                       );

} else {

  include_once('prod.dbinfo.inc');
  include_once('prod.pathinfo.inc');

  $debugConfig = array(
                       'groupByClass' => FALSE, //if output handles grouping, group logs by classname.
                       'debugLevel' => DEBUG_OFF,
                       'handleErrors' => FALSE,
                       'showIncludedFiles' => FALSE,
                       'detailedOutput' => FALSE
                       );

}

/* What level is an admin user? */
define( ANAPPADMIN, 1 );

/* Default long page title */
$defaultTitle = '@defaulttitle@';


/* Application name (used for cookies) */
$appName = '@appname@';


/* Global Templates */
$loginTemplate = "login.ihtml";
$loggedInTemplate = "index.ihtml";

/* Allow broker to handle requests for non-logged in people: */
$allowNonLoggedIn = FALSE; // Note, if you set this to true, your components must be cognizant of loggedin or not.

/* Required Variables used by cookieLogin: */ 

/* Use cookieLogin */
$loginModule = "cookieLogin";
$loginSafetyLevel = 2; //iform style safety level

/* User Login Expiration Time */
$loginExpires = 60*60*8; //8 hours

/* Cookie User Key (name of cookie to store loginName in) */
$cookieUserKey = '@appname@User';

/* Cookie Token Key (name of cookie to store auth token in) */
$cookieTokenKey = '@appname@Token';

/* Optional Variables used by cookieLogin */

/* Cookie Path Var (see php function set_cookie) */
$cookiePath = $self;

/* Cookie Domain Var (see php function set_cookie) */
$cookieDomain = ''; // '' signifies current URL domain

/* Use ssl only? */
$cookieSecure = FALSE;

/* Redirect to this after logging out. */
$logoutURL = $appURL;

/* Redirect to this after successful login */
$loginURL = $appURL;

/* End cookieLogin Vars */


/* Extra javascript files */
$javascripts = array('../firebug/firebugx.js');

/* Extra css files */
$styles = array();

/* Class called if the requested widget class can't be found. Useful for 404s, static html pages, etc. */
$defaultWidget = '404';

/* API Configuration */
$apiName = 'anApp'; 
$apiDescriptor = 'anApp.API';

/* Publicly available API Classes */
$publicAPIMethods = array(
  //  'noteAPI' => array(
  //  'serverName' => 'Note'
  // ),
);

/* All API classes (private) */
$apiMethods = array(
  //  'noteAPI' => array(
  //  'serverName' => 'Note'
  // ),
);

if ($environment == "dev") {
  $apiMethods['testAPI'] = array(
    'serverName' => 'Test'
  );
}
?>
