<?xml version="1.0" encoding="UTF-8"?>
<!-- ============================================================ -->
<!-- pinEntryScreen.gml                              -->
<!-- ============================================================ -->
<!-- ============================================================ -->
<!-- NOTE: You can optionally associate an "<idname>" element to  -->
<!-- any of the widgets.                                          -->
<!-- The "<idname>" element is required if the application        -->
<!-- business part (written in "C" code) needs to interact with   -->
<!-- the widget.                                                  -->
<!-- The value inside the "<idname>" element must be one of the   -->
<!-- "idname" attributes declared in the "widgetMappings.iml"     -->
<!-- file (located at project root). Please complete this file    -->
<!-- if needed.                                                   -->
<!-- The uniqueness of each <idname> is automatically checked     -->
<!-- (in the editor and at build time).                           -->
<!-- ============================================================ -->
<DAL xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://www.ingenico.com/schemas/dal/1_0">
  <version DAL_version="01.00" this_version="01.00"/>
  <goal name="pinEntryScreen">
    <window>
      <backalign>left</backalign>
      <forealign>left</forealign>
      <textalign>left</textalign>
      <paddings>
        <left>10</left>
        <top>0</top>
        <right>0</right>
        <bottom>0</bottom>
        <unit>perthousand</unit>
      </paddings>
      <children>
        <layout>
          <idname>widgetMappings.iml#DATE_SCREEN_BUTTON_OK</idname>
          <forealign>top</forealign>
          <children>
            <label>
              <grow>width</grow>
              <backalign>left</backalign>
              <forealign>left</forealign>
              <paddings>
                <left>0</left>
                <top>0</top>
                <right>0</right>
                <bottom>0</bottom>
                <unit>%</unit>
              </paddings>
              <text>SALE</text>
            </label>
            <label>
              <idname>../widgetMappings.iml#PIN_ENTRY__AMOUNT</idname>
              <item>
                <column>1</column>
                <row>0</row>
              </item>
              <backalign>right</backalign>
              <forealign>right</forealign>
              <paddings>
                <left>0</left>
                <top>0</top>
                <right>0</right>
                <bottom>0</bottom>
                <unit>%</unit>
              </paddings>
              <text>INR 99999999.00</text>
            </label>
          </children>
        </layout>
        <label>
          <idname>../widgetMappings.iml#PIN_ENTRY__HELP_MESSAGE</idname>
          <backalign>bottom</backalign>
          <forealign>center</forealign>
          <paddings>
            <left>0</left>
            <top>0</top>
            <right>0</right>
            <bottom>10</bottom>
            <unit>perthousand</unit>
          </paddings>
          <text>Last Attempts</text>
        </label>
        <label>
          <idname>widgetMappings.iml#PIN_ENTRY__MAIN_MESSAGE</idname>
          <textalign>center</textalign>
          <text>Please enter your PIN</text>
          <font>
            <style>normal</style>
            <size>
              <height>SMALL</height>
            </size>
          </font>
        </label>
        <label>
          <idname>widgetMappings.iml#PIN_ENTRY_MASK</idname>
          <item>
            <column>0</column>
            <row>2</row>
          </item>
          <textalign>center</textalign>
          <paddings>
            <left>0</left>
            <top>45</top>
            <right>0</right>
            <bottom>0</bottom>
            <unit>%</unit>
          </paddings>
          <text>* * * *</text>
          <font>
            <style>bold</style>
            <size>
              <height>LARGE</height>
            </size>
          </font>
        </label>
      </children>
    </window>
  </goal>
</DAL>