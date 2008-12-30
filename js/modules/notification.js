
/**
 * 
 * @class anApp.notification
 * @extends Ext.ux.ToastWindow
 *
 */

anApp.notification = function(config) {
    Ext.apply(this, config, {iconCls:'help'});
   anApp.notification.superclass.constructor.call(this);
    if (this.autoShow) {
	this.show();
    }
    console.log('Created %o', this);
}


/** @private */
Ext.extend(anApp.notification, Ext.ux.ToastWindow, {});
Ext.reg('notification', anApp.notification);
