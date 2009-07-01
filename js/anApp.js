
// create namespace
Ext.namespace('anApp');
Ext.namespace('Ext.ux'); //Get that out of the way.

/**
 *
 * @class anApp
 * @extends Ext.util.Observable
 *
 */


anApp = function(){
  /* Start Private Properties/Methods */

  /* End Private Properties/Methods */

    return Ext.apply(new Ext.util.Observable, {
  /* Start Public Properties/Methods */
	lazyLoading: true,
	moduleURLPrefix: 'js/modules/',
	appLayout: true,
	preloadModules: [],
	loadDependencies: true,
	loading: 0,
	events : {
	applayout: true,
	loaded: true
	},
    init : function() {
	Ext.QuickTips.init();
	if (this.lazyLoading) {
	    Ext.ComponentMgr.allowJSRemoteLoad = true;
	    Ext.ComponentMgr.allowCSSRemoteLoad = true;
	    Ext.ComponentMgr.remoteLoadBaseURL = this.moduleURLPrefix;
	}
	Ext.ComponentMgr.monitorAjax();

	if (this.appLayout) {
	    this.on('loaded', function() {
		var req = 		{
		    url: 'appLayout',
		    callback: this.doAppLayout,
		    scope: this,
		    params: {
			origURL: window.location.href
		    }
		};
		Ext.Ajax.request(req);
	    }, this, { single: true });
	}
	if (this.loadDependencies) {
	  this.autoLoadRemote('dependencies', false);
	}
        if (this.preloadModules.length > 0) {
          for (var i = 0; i < this.preloadModules.length; i++) {
	    this.autoLoadRemote(this.preloadModules[i], true);
	  }
        } else {
          this.fireEvent('loaded');
        }
    },
	autoLoadRemote: function(module, css) {
	    Ext.ComponentMgr.loadRemote(module, css, this);
	},
	doAppLayout : function(request,success,response) {
	    if (! this.layout ) {
	    var r;
	    if (response.responseText) {
		if (response.responseText.substr(0,1) == '{') {
	    	    r = Ext.util.JSON.decode(response.responseText);
		}
	    } else {
		r = response;
	    }
	    if (r.appLayout) {
		this.layout = new Ext.Viewport(r.appLayout);
		this.fireEvent('createlayout', r.appLayout, this);
		this.layout.on('show', function(e) {
		    this.fireEvent('applayout', this);
		}, this);
	    }
	}
	}
	/* End Public Properties/Methods */
    })
}();