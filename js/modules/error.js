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
      Ext.MessageBox.show(config);
    }
  this.destroy();
}


/** @private */
Ext.extend(anApp.error, Ext.Component, {});

Ext.reg('error', anApp.error);
