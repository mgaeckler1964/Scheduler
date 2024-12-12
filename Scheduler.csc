<?xml version="1.0" encoding="iso-8859-1" ?>
<setup>
<application title="MG Terminverwaltung" version="1.0.0.303" author="gak" />
<bde AliasName="SCHEDULE" RESOURCE="0007" DataPath="" dbVersion="2" />
<icons Group="MG Scheduler">
<icon title="Terminverwaltung" target="PROGRAM\scheduler.exe" main="true" dbUpd="" />
<icon title="TerminDB Verwaltung" target="PROGRAM\schedulerDB.exe" main="" dbUpd="true" />
<icon title="BDE Verwaltung" target="BDE\BDEADMIN.EXE" main="" dbUpd="" />
<icon title="Mobile Zeiterfassung" target="PROGRAM\mobil.exe" main="" dbUpd="" />
<icon title="Handbuch" target="PROGRAM\scheduler.pdf" main="" dbUpd="" />
<icon title="Datenpumpe" target="BDE\datapump.exe" main="" dbUpd="" />
</icons>
<source>
<group name="PROGRAM" destination="$ProgramFiles32">
<file source="..\..\Object\scheduler.exe" />
<file source="..\..\Object\schedulerDB.exe" />
<file source="..\..\Object\Mobil.exe" />
<file source="..\..\Object\java\mobil.jad" />
<file source="..\..\Object\java\mobil.jar" />
<file source="..\..\Object\java\MobilAndroid.apk" />
<file source="..\..\Object\MobilMac.dmg" />
<file source="scheduler.pdf" />
</group>
</source>
</setup>