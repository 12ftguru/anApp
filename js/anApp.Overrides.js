Ext.override(Ext.Component, {
    /**
     * Update the component with a config similar to the creation
     * config, except that methods may be called as well...
     * Example:
     * {
     *   property1: 'value',
     *   methodOne: [arg1, arg2, arg3],
     *   methodTwo: {
     *               arguments: [arg1, arg2, arg3],
     *               scope: 'id-of-other-component'
     *              }
     * Note, you cannot chain functions (myComponent.methodOne().methodTwo()
     * @methodOf Ext.Component
     * @name Ext.Component#updateComponent
     * @return {void}
     */
    updateComponent : function(config) {
	for (var property in config) {
	    if (typeof this[property] == 'function') {
		if (config[property].scope) {
		    this[property].apply(Ext.getCmp(config[property].scope), config[property].arguments);
		} else {
		    this[property].apply(this, config[property]);
		}
	    } else {
		this[property] = config[property];
	    }
	}
    }
});

Ext.override(Ext.layout.CardLayout, {
	      onLayout : function(ct, target){
		Ext.layout.CardLayout.superclass.onLayout.call(this, ct, target);
		this.container.setActiveItem = function(item) {
		  if (this.layout) {
		    this.layout.setActiveItem(item);
		  } else {
		    this.setActiveItem(item);
		  }
		};
	      }
	     });


Ext.apply(Ext.ComponentMgr, {
	/**
         * Are we currently monitoring ajax connections for create/update statements?
         */
        monitoringAjax: false,
	/**
         * Allow remote loading of missing components.
         */
	allowJSRemoteLoad: false,
        /**
         * Load corresponding css when you load a missing component.
         */
	allowCSSRemoteLoad: false,
        /**
         * Base url for remote js loading. Must end with trailing slash.
         */
	remoteLoadBaseURL: '',
        /**
         * Private cache of loaded xtypes (we only load once, this is to avoid a race condition).
         */
	types: new Ext.util.MixedCollection(),
    groups: new Ext.util.MixedCollection(),
        loading: 0,
   /**
    * Instruct the ComponentMgr to toggle monitoring of ajax responses for
    *  'update' and 'create' statements:
    * Example Ajax Response:
    * {
    *   success: true,
    *   update: {
    *            'componentID-1' : { updateComponent Spec },
    *            'componentID-2' : { updateComponent Spec }
    *   },
    *  create: [ {valid xtype spec}, {valid xtype spec} ]
    * }
    * If you give your create xtype specs a "parentID", that
    * component's add function will be called with the spec, otherwise
    * it will be added via the Ext.ComponentMgr.create();
    * @methodOf Ext.ComponentMgr
    * @name Ext.ComponentMgr#monitorAjax
    */
    monitorAjax: function() {
	if (this.monitoringAjax) {
	    this.monitoringAjax = false;
	    Ext.Ajax.un('requestcomplete', this.monitorAjaxResponseHandler);
	} else {
	    this.monitoringAjax = true;
	    Ext.Ajax.on('requestcomplete', this.monitorAjaxResponseHandler);
	}
    },
    /**
     * Response handler installed by monitorAjax. Not for you.
     * @methodOf Ext.ComponentMgr
     * @name Ext.ComponentMgr#monitorAjaxResponseHandler
     */
    monitorAjaxResponseHandler: function(connection, response, requestOptions) {
	var obj;
	if (response.responseText) {
		if (response.responseText.substr(0,1) == '{') {
	    	    obj = Ext.util.JSON.decode(response.responseText);
		}
	} else {
	    obj = response;
	}
	if (obj) {
	if (obj.create) {
	    Ext.ComponentMgr.createComponents(obj.create);
	}
	if (obj.update) {
	    Ext.ComponentMgr.updateComponents(obj.update);
	}
	}
    },
    /**

     * Updates a set of components via an update config:
     * {
     *            'componentID-1' : { updateComponent Spec },
     *            'componentID-2' : { updateComponent Spec }
     *   }
     * Fires the 'update' event after each component is updated.
     * @methodOf Ext.ComponentMgr
     * @name Ext.ComponentMgr#updateComponents
     * @param {Object} update The update config.
     */
    updateComponents: function(update) {
	for (var cmpID in update) {
	    var cmp = Ext.getCmp(cmpID);
	    if (cmp && cmp.updateComponent) {
		cmp.updateComponent(update[cmpID]);

	    }
	}
    },

    /**
     * Allows you to run a function when a specified xtype is available.
     * @param {String} xtype The xtype in question
     * @param {Function} fn The callback function
     * @param {Object} scope The scope of the callback
     */
    onXTypeAvailable: function(xtype, fn, scope) {
	if (this.types.containsKey('xtype')) {
	    fn.call(scope || xtype, xtype);
	} else {
	    this.types.on("add", function(idx, obj, key) {
		fn.call(scope || key, key);
	    }, this, {single: true});
        }
    },

    /**
     * Gets a group of components by groupID
     * @param {String} groupID The groupID in question
     * @return {MixedCollection} Matching items
     */

    getGroup: function(groupID) {
	return Ext.ComponentMgr.groups.filter('groupID', groupID, false, true);
    }


        /**
         * Registers a new Component constructor, keyed by a new
         * {@link Ext.Component#xtype}.<br><br>
         * Use this method to register new subclasses of {@link Ext.Component} so
         * that lazy instantiation may be used when specifying child Components.
         * see {@link Ext.Container#items}
         * @param {String} xtype The mnemonic string by which the Component class
         * may be looked up.
         * @param {Constructor} cls The new Component class.
         */


    /**
     * Creates a set of components via a create config:
     *  [ {valid xtype spec}, {valid xtype spec} ]
     * If you give your create xtype specs a "parentID", that
     * component's add function will be called with the spec, otherwise
     * it will be added via the Ext.ComponentMgr.create();
     * Fires the add event after each component is added.
     * @methodOf Ext.ComponentMgr
     * @name Ext.ComponentMgr#createComponents
     * @param {Array} components Array of xtype configs to create
     */
    createComponents: function(components) {
	var i;
	if (typeof components == "string" && this != Ext.ComponentMgr && Ext.ComponentMgr.hasXType(components)) { //called from some other scope
	    components = this;
	}
	for (i = 0; i < components.length; i++) {
	    if (components[i].parentID) {
		var parentCmp = Ext.getCmp(components[i].parentID);
		try {
		    var cmp = parentCmp.add(components[i]);
		    if (cmp.groupID) {
			Ext.ComponentMgr.groups.add(cmp);
		    }
		} catch (err) {
		    Ext.ComponentMgr.loadRemote(components[i]);
		}
	    } else if (components[i].xtype) {
		try {
		    var cmp = Ext.ComponentMgr.create(components[i]);
		    if (cmp.groupID) {
			Ext.ComponentMgr.groups.add(cmp);
		    }
		} catch (err) {
		    Ext.ComponentMgr.loadRemote(components[i]);
		}
	    }
	}
    },

    /**
     * Given a config, loads a remote js (and optionally css) file, then attempts to create the config.
     */
    loadRemote : function(config, css, relayObject){
	var xtype;
	if (typeof config == "object" && config.xtype) {
	    xtype = config.xtype;
	    this.onXTypeAvailable(xtype, Ext.ComponentMgr.createComponents, [config]);
	} else {
	    xtype = config;
	}
	if (this.allowJSRemoteLoad && !this.hasXType(xtype)){
	    var s = document.createElement("script");
	    s.type = "text/javascript";
	    s.src = this.remoteLoadBaseURL+xtype+'.js';
	    this.loading++;

	    var head = document.getElementsByTagName("head")[0];
	    head.appendChild(s);
	    var self = this;
	    s.onload = s.onreadystatechange = function() {
		if (this.readyState && this.readyState == "loading") {
		    return;
		} else {
		    self.loading--;
		    if (self.loading == 0 && relayObject && relayObject.fireEvent) {
			relayObject.fireEvent('loaded');
		    }

		}
	    };
	    if (css || (css !== false && this.allowCSSRemoteLoad)) {
		/* Also load matching css */
		var c = document.createElement('link');
		c.type = 'text/css';
		c.rel = 'stylesheet';
		c.href = this.remoteLoadBaseURL+xtype+'.css';
		c.media = 'screen';
		head.appendChild(c);
	    }
	    return false;
	}
    }
});

Ext.ComponentMgr.registerType = Ext.ComponentMgr.registerType.createSequence(function(xtype, cls) {
    Ext.ComponentMgr.types.add(xtype,cls);
});
Ext.reg = Ext.ComponentMgr.registerType;

    if (! Ext.ComponentMgr.hasXType) {
	Ext.ComponentMgr.hasXType = function(xtype) {
	    return Ext.ComponentMgr.types.containsKey(xtype);
	};
    }