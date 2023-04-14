t <html><head><title>Control de Temperatura</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var TempUpdate = new periodicObj("Temp.cgx", 1000);
t function updateValueTemp() {
t  TempVal = document.getElementById("temp").value;
t }
t function updateTemp() {
t  Val_temp = document.getElementById("temp").value;
t }
t function periodicUpdateTemp() {
t  if(document.getElementById("adChkBox").checked == true) {
t   updateMultiple(TempUpdate,updateValueTemp);
t   rtc_elTemp = setTimeout(periodicUpdateTemp, TempUpdate.period);
t  }
t  else
t   clearTimeout(rtc_elTemp);
t }
t  </script> </head>
i pg_header.inc
t <h2 align=center><br>Modulo de Temperatura</h2>
t <p><font size="2">Esta página muestra el valor de la temperatura.
t <form action="Temp.cgi" method="post" name="temperatura">
t <input type="hidden" value="temperatura" name="pg">
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#FFA420>
t  <th width=30%>Temperatura</th>
t <tr>
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px text-align:"center""
c t size="60" id="temp" value="%5.3f ºC"></td>
t </font></table>
t <p align=center>
t <input type=button value="Actualizar Temperatura" onclick="updateMultiple(updateTemp,updateValueTemp)">
t <input type="checkbox" id="adChkBox" onclick="periodicUpdateTemp()"> Actualización periódica
t </p></form>
i pg_footer.inc
. End of script must be closed with period.