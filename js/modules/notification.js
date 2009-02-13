
/**
 *
 * @class anApp.notification
 * @extends Ext.Component
 *
 * This is silly. Ext.MessageBox is not a component, so you gotta call it by hand.
 *
 */

Ext.namespace('anApp');

anApp.notification = function(config) {
  Ext.apply(this, config, {iconCls:'help'});
  anApp.notification.superclass.constructor.call(this);
  if (this.autoShow) {
	Ext.MessageBox.show(config);
    }
    console.log('Created %o', this);
  this.destroy();
}


/** @private */
Ext.extend(anApp.notification, Ext.Component, {});
Ext.reg('notification', anApp.notification);
