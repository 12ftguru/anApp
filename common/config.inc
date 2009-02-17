<?php

include_once('dbinfo.inc');
include_once('pathinfo.inc');

/* What level is an admin user? */
define( ANAPPADMIN, 1 );

/* Are we in production? */
$deployed = FALSE;

/* Default long page title */
$defaultTitle = '';

/* Application name (used for cookies) */
$appName = 'anAppTest';

/* What's our root directory? */
$self = '/anapp/';

/* Full URL for our app: */
$appURL = 'http://127.0.0.1'.$self;

/* Global Templates */
$loginTemplate = "login.ihtml";
$loggedInTemplate = "index.ihtml";

/* Required Variables used by cookieLogin: */ 

/* Use cookieLogin */
$loginModule = "cookieLogin";
$loginSafetyLevel = 2; //iform style safety level

/* User Login Expiration Time */
$loginExpires = 60*60*8; //8 hours

/* Cookie User Key (name of cookie to store loginName in) */
$cookieUserKey = 'anAppTestUser';

/* Cookie Token Key (name of cookie to store auth token in) */
$cookieTokenKey = 'anAppTestToken';

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
$defaultWidget = 'testbutton2';

?>
