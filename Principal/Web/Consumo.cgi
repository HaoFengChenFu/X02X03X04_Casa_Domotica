t <html><head><title>Control del Consumo</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var ConsumoUpdate = new periodicObj("Consumo.cgx", 1000);
t function updateValueConsumo() {
t  ConsumoVal = document.getElementById("Consumo_value").value;
t }
t function periodicUpdateConsumo() {
t  if(document.getElementById("adChkBox").checked == true) {
t   updateMultiple(ConsumoUpdate,updateValueConsumo);
t   Consumo_elConsumo = setTimeout(periodicUpdateConsumo, ConsumoUpdate.period);
t  }
t  else
t   clearTimeout(Consumo_elConsumo);
t }
t  </script> </head>
i pg_header.inc
t <h2 align=center><br>Modulo del consumo</h2>
t <p><font size="2">Esta página muestra el valor del <b>Consumo</b>.
t <form action="Consumo.cgi" method="post" name="Consumo">
t <input type="hidden" value="Consumo" name="pg">
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#FFA420>
t  <th width=100%>Consumo</th>
t <tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px text-align:"center""
c i 1 size="27" id="Consumo_value" value="%s"></td></tr>
t </font></table>
t <p align=center>
t <input type=button value="Refresh" onclick="updateMultiple(ConsumoUpdate,updateValueConsumo)">
t <input type="checkbox" id="adChkBox" onclick="periodicUpdateConsumo()"> Actualización periódica
t </p></form>
i pg_footer.inc
. End of script must be closed with period.