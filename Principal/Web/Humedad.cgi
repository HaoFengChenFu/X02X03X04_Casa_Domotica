t <html><head><title>Control de Humedad</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var HumedadUpdate = new periodicObj("Humedad.cgx", 1000);
t function updateValueHumedad() {
t  HumedadVal = document.getElementById("valor_humedad").value;
t }
t function periodicUpdateHumedad() {
t  if(document.getElementById("adChkBox").checked == true) {
t   updateMultiple(HumedadUpdate,updateValueHumedad);
t   val_elHumedad = setTimeout(periodicUpdateHumedad, HumedadUpdate.period);
t  }
t  else
t   clearTimeout(val_elHumedad);
t }
t  </script> </head>
i pg_header.inc
t <h2 align=center><br>Modulo de Humedad</h2>
t <p><font size="2">Esta página muestra el valor de la humedad relativa.
t <form action="Humedad.cgi" method="post" name="humedad">
t <input type="hidden" value="humedad" name="pg">
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#00AAE4>
t  <th width=30%>Humedad</th>
t <tr>
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px text-align:"center""
c h size="60" id="valor_humedad" value="%5.2f %%"></td>
t </font></table>
t <p align=center>
t <input type=button value="Actualizar Humedad" onclick="updateMultiple(updateHumedad,updateValueHumedad)">
t <input type="checkbox" id="adChkBox" onclick="periodicUpdateHumedad()"> Actualización periódica
t </p></form>
i pg_footer.inc
. End of script must be closed with period.