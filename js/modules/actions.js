anApp.testButton1 = new Ext.Action(
    {
	text: 'Do something',
	handler: function(){
	    Ext.Ajax.request({url: 'testbutton'});
	}
    }
);

anApp.testButton2 = new Ext.Action(
    {
	text: 'Error Window',
	handler: function(){
	    Ext.Ajax.request({url: 'testbutton2'});
	}
    }
);

anApp.testButton3 = new Ext.Action(
    {
	text: 'Default Widget Test',
	handler: function(){
	    Ext.Ajax.request({url: 'testbutton3'});
	}
    }
);