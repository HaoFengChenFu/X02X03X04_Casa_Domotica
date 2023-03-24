t <html><head><title>RTC Control</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var pot1Update = new periodicObj("pot1.cgx", 500);
t var pot2Update = new periodicObj("pot2.cgx", 500);
t var rtcUpdate = new periodicObj("time.cgx", 1000);
t function updateValueRTC() {
t  rtcTimeVal = document.getElementById("rtc_time").value;
t }
t function updatePot() {
t  adVal_1 = document.getElementById("pot1_value").value;
t  adVal_2 = document.getElementById("pot2_value").value;
t }
t function periodicUpdateRTC() {
t  if(document.getElementById("adChkBox").checked == true) {
t   updateMultiple(rtcUpdate,updateValueRTC);
t   rtc_elTime = setTimeout(periodicUpdateRTC, rtcUpdate.period);
t  }
t  else
t   clearTimeout(rtc_elTime);
t }
t  </script> </head>
i pg_header.inc
t <h2 align=center><br>Configuracion</h2>
t <p><font size="2">Esta página muestra el valor del tiempo y del ADC.
t <form action="Mezcla.cgi" method="post" name="mezcla">
t <input type="hidden" value="mezcla" name="pg">
#
#    Las 2 lineas siguientes son para definir el largo de la celda y el color
#
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=20%>Tiempo</th>
t <tr>
# text-align "center" pone el texto en el centro
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px text-align:"center"
c h 1 size="60" id="rtc_time" value="%s" text-align="center"></td>
t </font></table>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#30FF24>
t  <th width=30%>Valor ADC 1</th>
t  <th width=30%>Valor ADC 2</th>
t <tr>
#
# Para el potenciometro 1
# <td align="center">   Esto esta causando problemas, se podria quitar el align 
#
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px text-align:"center""
c g 2 size="60" id="pot1_value" value="%5.3f V"></td>
#
# Para el potenciometro 2
#
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px text-align:"center""
c k 2 size="60" id="pot2_value" value="%5.3f V"></td></tr>
t </font></table>
t <p align=center>
t <input type=button value="Refresh Time" onclick="updateMultiple(rtcUpdate,updateValueRTC)">
t <input type=button value="Refresh Pot1" onclick="updateMultiple(pot1Update,updatePot)">
t <input type=button value="Refresh Pot2" onclick="updateMultiple(pot2Update,updatePot)">
t Periodic:<input type="checkbox" id="adChkBox" onclick="periodicUpdateRTC()">
t <br align="center">
t <input type=submit name=SetTime value="SetTime" id="valor">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.