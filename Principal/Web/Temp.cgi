t <html><head><title>Control de Temperatura</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var tempUpdate = new periodicObj("Temp.cgx", 1000);
t function updateValueTemp() {
t  TempVal = document.getElementById("temp_value").value;
t }
t function periodicUpdateTemp() {
t  if(document.getElementById("adChkBox").checked == true) {
t   updateMultiple(tempUpdate,updateValueTemp);
t   Temp_elTemp = setTimeout(periodicUpdateTemp, tempUpdate.period);
t  }
t  else
t   clearTimeout(Temp_elTemp);
t }
t  </script> </head>
i pg_header.inc
t <h2 align=center><br>Modulo de Control de Temperatura</h2>
t <p><font size="2">Esta página muestra el valor de la <b>Temperatura</b>.
t <form action="Temp.cgi" method="post" name="Temp">
t <input type="hidden" value="Temp" name="pg">
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#FFA420>
t  <th width=100%>Temperatura</th>
t <tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px text-align:"center""
c t 1 size="27" id="temp_value" value="%s"></td></tr>
t </font></table>
t <p align=center>
t <input type=button value="Refresh" onclick="updateMultiple(tempUpdate,updateValueTemp)">
t Periodic:<input type="checkbox" id="adChkBox" onclick="periodicUpdateTemp()">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.