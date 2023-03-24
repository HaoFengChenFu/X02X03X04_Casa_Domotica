t <html><head><title>RTC Control</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var rtcTimeUpdate = new periodicObj("time.cgx", 1000);
t function updateValueRTC() {
t  rtcTimeVal = document.getElementById("rtc_time").value;
t }
t function periodicUpdateRTC() {
t  if(document.getElementById("adChkBox").checked == true) {
t   updateMultiple(rtcTimeUpdate,updateValueRTC);
t   rtc_elTime = setTimeout(periodicUpdateRTC, rtcTimeUpdate.period);
t  }
t  else
t   clearTimeout(rtc_elTime);
t }
t  </script> </head>
i pg_header.inc
t <h2 align=center><br>Modulo de Control del RTC</h2>
t <p><font size="2">Esta página muestra el valor del tiempo y la fecha del <b>Modulo RTC</b>.
t <form action="rtc.cgi" method="post" name="rtc">
t <input type="hidden" value="rtc" name="pg">
#
#    Las 2 lineas siguientes son para definir el largo de la celda y el color
#
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=50%>Tiempo y Fecha</th>
t <tr>
# text-align "center" pone el texto en el centro
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
c h 1 size="30" id="rtc_time" value="%s" maxlength=40></td>
t </font></table>
t <p align=center>
t <input type=button value="Refresh" onclick="updateMultiple(rtcTimeUpdate,updateValueRTC)">
t Periodic:<input type="checkbox" id="adChkBox" onclick="periodicUpdateRTC()">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.
