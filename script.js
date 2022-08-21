//function showFiles() {
//	var files =  JSTools.dirEntryList(".", "*"); 
//	JSTools.alert(files.join("\n") );
//} 
var minimum_height = 0;
function sbros(){
    if(JSTools.alert(decodeURIComponent(escape("Сброс"))) === 16384){
		sec.clearTimer();
		np_red.sbros();
		np_blue.sbros();
		nv_red.sbros();
		nv_blue.sbros();
		ball_red.sbros();
		ball_blue.sbros();
		akt_red.sbros();
		akt_blue.sbros();
        plus_red.setData("");
        plus_blue.setData("");
	}
}
function Plus_red(){
    if(plus_red.Text === "+")
        plus_red.setData("");
    else if(plus_blue.Text === "")
        plus_red.setData("+");
}
function Plus_blue(){
    /*
    if(plus_blue.Text === "+"){
        plus_blue.setData("");
        sd_plus_blue.setData("");
    }else if(plus_red.Text === ""){
        plus_blue.setData("+");
        sd_plus_blue.setData("+");
    }
    */
    JSTools.alert("plus");
}
function parter_red(){
    if(sec_red.Visible === false)
		sec_red.setVisible(true);
	else
		sec_red.setVisible(false);
}
function parter_blue(){
    if(sec_blue.Visible === false)
		sec_blue.setVisible(true);
	else
		sec_blue.setVisible(false);
}
function keyEvent(key){
	switch(key) {
		case 16777219:  //backspace 
			sbros();
		break;
		case 16777216:  //esc
            if(JSTools.alert(decodeURIComponent(escape("Выход"))) === 16384){
				JSTools.quitApplication();
			}
			break
		/*
		case 56:
			minimum_height += 1;
			grid.setRowMinimumHeight(0, minimum_height);
			grid.setRowMinimumHeight(1, minimum_height);
			grid.setRowMinimumHeight(2, minimum_height);
			grid.setRowMinimumHeight(3, minimum_height);
			grid.setRowMinimumHeight(4, minimum_height);
			grid.setRowMinimumHeight(5, minimum_height);
		break
		case 50:
			if(minimum_height > 0){
				minimum_height = 1;
				grid.setRowMinimumHeight(0, minimum_height);
				grid.setRowMinimumHeight(1, minimum_height);
				grid.setRowMinimumHeight(2, minimum_height);
				grid.setRowMinimumHeight(3, minimum_height);
				grid.setRowMinimumHeight(4, minimum_height);
				grid.setRowMinimumHeight(5, minimum_height);
			}
			*/
		
		default:
		break
	}
}

function set_family(a, b){
	var st;
    if(a !=="-" && a !== ""){
		st	= a.split(";");
        fam_red.text(st[0].split(" ")[0]);
        reg_red.text(st[1].toUpperCase());
		wgt.setFlagRed(st[1]);
	}else {
        fam_red.text("");
        reg_red.text("")
	}
    if(b !== "-" && b !== ""){
		st = b.split(";");
        fam_blue.text(st[0].split(" ")[0]);
        reg_blue.text(st[1].toUpperCase());
		wgt.setFlagBlue(st[1]);
	}else {
        fam_blue.text("");
        reg_blue.text("");
	}
}
function weight(w){
	cat.setText(w);
}
//JSTools.print("JSTools test!"); 
//cmdFiles.clicked.connect(showFiles); 
//cmdQuit.clicked.connect(function() {JSTools.quitApplication()});

btnTime.clicked.connect(function() {sec.startstop()});
btnPlus_red.clicked.connect(Plus_red);
btnPlus_blue.clicked.connect(Plus_blue);
wgt.sendKey.connect(keyEvent);
//np_red.change_np.connect(np_blue.setValue);
btnParter_red.clicked.connect(parter_red);
btnParter_blue.clicked.connect(parter_blue);
btnSettings.clicked.connect(function() {lf.showFullScreen();});
lf.sig_hide.connect(set_family);
lf.sig_category.connect(weight);
