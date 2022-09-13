var minimum_height = 0;
function sbros(){
    if(JSTools.alert(decodeURIComponent(escape("Сброс"))) === 16384){
		mainTimer.Reset();
        sec_red.Reset();
        sec_blue.Reset();
        sec_red_t.Reset();
        sec_blue_t.Reset();
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
    if(plus_red.text === "+")
        plus_red.setData("");
    else if(plus_blue.text === "")
        plus_red.setData("+");
}
function Plus_blue(){
    if(plus_blue.Text === "+")
        plus_blue.setData("");
    else if(plus_red.Text === "")
        plus_blue.setData("+");
}
function fightTime(){
    if(sec_red_t.Status == 1 || sec_blue_t.Status == 1)
        return;
    if(sec_red_t.Visible || sec_blue_t.Visible){
        sec_red_t.setVisible(false);
        sec_red_t.sigVisible(false);
        sec_blue_t.setVisible(false);
        sec_blue_t.sigVisible(false);
        return;
    }
    if(mainTimer.Status == 1){
        if(sec_red.Status == 1)
            sec_red.StartStop();
        if(sec_blue.Status == 1)
            sec_blue.StartStop();
        mainTimer.StartStop();
    }else{
        if(sec_red.Visible || sec_blue.Visible){
            sec_red.setVisible(false);
            sec_red.sigVisible(false);
            sec_blue.setVisible(false);
            sec_blue.sigVisible(false);
        }else{
            mainTimer.StartStop();
            sec_red.Reset();
            sec_blue.Reset();
        }
    }
}

function parter_red(){
    if(sec_red_t.Visible || sec_blue_t.Visible)
        return;
    if(sec_red.Visible === false){
		sec_red.setVisible(true);
        if(mainTimer.Status == 1)
            sec_red.StartStop();
        else
            sec_red.sigVisible(true);
    }else{
        if(mainTimer.Status == 1){
            if(!sec_red.Status == 1){
                sec_red.Reset();
            }
            sec_red.StartStop();
        }else{
            sec_red.setVisible(false);
            sec_red.sigVisible(false);
        }
    }
}
function parter_blue(){
    if(sec_red_t.Visible || sec_blue_t.Visible)
        return;
    if(sec_blue.Visible === false){
        sec_blue.setVisible(true);
        if(mainTimer.Status == 1)
            sec_blue.StartStop();
        else
            sec_blue.sigVisible(true);
    }else{
        if(mainTimer.Status == 1){
            if(!sec_blue.Status == 1){
                sec_blue.Reset();
            }
            sec_blue.StartStop();
        }else{
            sec_blue.setVisible(false);
            sec_blue.sigVisible(false);
        }
    }
}
function t_red(){
    if(!mainTimer.Status == 1 && !sec_red.Visible && !sec_blue.Visible){
        sec_red_t.setVisible(true);
        sec_red_t.StartStop();
    }
}
function t_blue(){
    if(!mainTimer.Status == 1 && !sec_red.Visible && !sec_blue.Visible){
        sec_blue_t.setVisible(true);
        sec_blue_t.StartStop();
    }
}
function keyEvent(key){
	switch(key) {
		case 16777219:  //backspace 
			sbros();
		break;
		case 16777216:  //esc
            if(JSTools.alert(decodeURIComponent(escape("Выход"))) === 16384){
				wgt.close();
			}
        break;
		default:
        break;
	}
}

function set_family(a, b){
	var st;
    if(a !=="-" && a !== ""){
		st	= a.split(";");
        fam_red.text = (st[0].split(" ")[0]);
        reg_red.text = (st[1].toUpperCase());
		wgt.setFlagRed(st[1]);
	}else {
        fam_red.text = ("");
        reg_red.text = ("")
	}
    if(b !== "-" && b !== ""){
		st = b.split(";");
        fam_blue.text = (st[0].split(" ")[0]);
        reg_blue.text = (st[1].toUpperCase());
		wgt.setFlagBlue(st[1]);
	}else {
        fam_blue.text = ("");
        reg_blue.text = ("");
	}
}

btnTime.clicked.connect(fightTime);
btnPlus_red.clicked.connect(Plus_red);
btnPlus_blue.clicked.connect(Plus_blue);
btnTehTime_red.clicked.connect(t_red);
btnTehTime_blue.clicked.connect(t_blue);
wgt.sendKey.connect(keyEvent);
btnParter_red.clicked.connect(parter_red);
btnParter_blue.clicked.connect(parter_blue);
btnSettings.clicked.connect(function() {lf.showFullScreen();});
lf.sig_hide.connect(set_family);