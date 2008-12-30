/**
 * 
 * @class anApp.error
 * @extends Ext.ux.ToastWindow
 *
 */

anApp.error = function(config) {
    Ext.apply(this, config, {iconCls:'error', noHide: true});
   anApp.error.superclass.constructor.call(this);
    if (this.autoShow) {
	this.show();
    }
}


/** @private */
Ext.extend(anApp.error, Ext.ux.ToastWindow, {});

Ext.reg('error', anApp.error);
