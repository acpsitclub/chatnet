<?php
/*
 * MIT License
 *
 * Copyright (c) 2021 Muhammad Bin Zafar <midnightquantumprogrammer@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */




$post = file_get_contents("php://input");
$post = json_decode($post);

if (isset($post->uSend)) $uSend = $post->uSend;
else $uSend = "";

if (isset($post->uRecv)) $uRecv = $post->uRecv;
else $uRecv = "";

if (isset($post->shkey)) $shkey = $post->shkey;
else $shkey = "";

if (isset($post->msgText)) $msgText = $post->msgText;
else $msgText = "";


$cLoc = "./.cache";
if (!file_exists($cLoc)) mkdir($cLoc);
$COMMANDS = ["!connect ", "!anyconnreq", "!anyconnreq ", "!anynewmsg", "!connaccept ", "!conndenied "];
validate_comm($uSend, $uRecv, $shkey, $msgText, $cLoc, $COMMANDS);





if (substr($msgText, 0, 9) === "!connect ") {
	// Send of conn req

	// uRecv is the person to whom the request is being sent.
	$uRecv = substr($msgText, 9);
	if ($uRecv === "") exit("-1");
	if ($uSend === "") exit("-1");
	$NewKey = new_shkey();
	$uRecvFn = "$cLoc/$uRecv-$NewKey";
	file_put_contents($uRecvFn, "$uSend");
	$uSendFn = "$cLoc/$uSend-$NewKey";

	$uRecvInteracted = false;
	for($i=0; $i<30; $i++) {
		sleep(1);
		if (file_exists($uSendFn)) {
			// uRecv created chatroom for uSend
			// !connaccept
			$uRecvInteracted = true;
			//unlink($uSendFn);
			//$chatroom = "$cLoc/$NewKey";
			
			//$chatroom = $uSendFn;
			//file_put_contents($chatroom, ""); #create chatroom for uSend - uRecv did
			echo $NewKey;
			break;
		}
		else if (!file_exists($uRecvFn)) {
			// uRecv deleted the chatroom, which uSend created.
			// !conndenied
			$uRecvInteracted = true;
			echo "0";
			break;
		}
	}
	if ($uRecvInteracted === false) {
		// Conn Req to uRecv timed out.
		unlink($uRecvFn);   //delete chatroom for uRecv
		echo "-2";
	}
}


else if(substr($msgText, 0, 11) === "!anyconnreq") {
	// Checking for connection requests.	
	
	//uSendFn: A file created for uSend -- by the person wanting to connect.
	if ($uSend === "") exit("-1");

	$uSendFnAll = glob("$cLoc/$uSend-*"); 
	if (count($uSendFnAll) > 0) {
		//There's a request.
		//uRecv: The person wanting to connect
		$uSendFn = $uSendFnAll[0];
		$uRecv = file_get_contents($uSendFn);  
		$uRecv = substr($uRecv, 0, strlen($uRecv));  # EOF causes newline.
		echo $uRecv;
	}
	else {
		//No conn requests.
		exit("0");
	}
}


else if (substr($msgText, 0, 12) === "!connaccept ") {
	// uSend accepted the conn req. So, create file for uRecv.
	if ($uSend === "") exit(-1);
	$uRecv = substr($msgText, 12);
	if ($uRecv === "") exit(-1);


	$uSendFn = glob("$cLoc/$uSend-*")[0]; 
	$cut = strlen("$cLoc/$uSend-");
	$Key = substr($uSendFn, $cut);

	# uRecv is the person wanted to connect initially.
	$uRecvFn = "$cLoc/$uRecv-$Key";
	file_put_contents($uRecvFn, ""); //the person is happy!
	
	#sleep(2);
	#unlink($uSendFn);  #Why delete chatroom?
	file_put_contents($uSendFn, "");   //bcz, uRecv had his name inside the file

	echo $Key;
}


else if (substr($msgText, 0, 12) === "!conndenied ") {
	// uSend denied the conn req. So, delete the file for uRecv.
	if ($uSend === "") exit(-1);
	//$uRecv = substr($msgText, 12);
	//if ($uRecv === "") exit(-1);

	$uSendFnAll = glob("$cLoc/$uSend-*");
	foreach($uSendFnAll as $uSendFn) {
		// All requests are denied.
		unlink($uSendFn);
	}
}


else if (substr($msgText, 0, 10) === "!anynewmsg") {
	// Checking for new messages.
	if ($uSend === "" || $shkey === "") exit("Error: While checking for new messages... uSend is empty.");
	$uSendFn = "$cLoc/$uSend-$shkey";
	$newMsg = file_get_contents($uSendFn);
	echo substr($newMsg, 0, strlen($newMsg));
}


else {
	// Send of a message
	if ($uRecv==="" || $shkey==="" || $msgText === "") {
		exit("Error: uSend/MsgText/shkey is empty.");
	}
	$uRecvFn = "$cLoc/$uRecv-$shkey";
	file_put_contents($uRecvFn, $msgText);
}




function new_shkey() {
	$randByte = random_bytes(20);

	date_default_timezone_set("Asia/Dhaka");
	$now = date("ymdHis");
	
	$key = bin2hex($randByte);
	$key = "$now-$key";
	return $key;
}


function validate_comm($uSend, $uRecv, $shkey, $msgText, $cLoc, $COMMANDS) {
	// This function checks if the parameters' values are acceptable.
	if ($uSend !== "") {
		if (strlen($uSend)>20) exit("Invalid uSend: $uSend");
		if (!ctype_alnum($uSend)) exit("Invalid uSend: $uSend");
	}

	if ($uRecv !== "") {
		if (strlen($uRecv)>20) exit("Invalid uRecv: $uRecv");
		if (!ctype_alnum($uRecv)) exit("Invalid uRecv: $uRecv");
	}

	// Except server-side commands,
	// you need a shkey to send msgText.
	$wasCommand = false;

	if ($shkey === "") {
		// No shkey, then it must be a cmd.
		foreach ($COMMANDS as $c) {
			if (substr($msgText, 0, strlen($c)) === $c) {
				// Okay, it was a command
				$wasCommand = true;
				break;
			}
		}
		if ($wasCommand === false) exit("[Error] Neither a message, nor a command.");
	}
	else {
		// So, there's a shkey given. It should be a sending-msg.		
		// Or... it could be a recieving-msg.
		if ($uRecv === "" || $uSend==="") exit("[Error] uSend/uRecv empty.");
		$uRecvFn = "$cLoc/$uRecv-$shkey";
		if (!file_exists($uRecvFn)) {
			exit("Invalid shkey: $shkey");
		}
	}
}
?>
