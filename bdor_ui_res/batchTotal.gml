<?xml version="1.0" encoding="UTF-8"?>
<!-- ============================================================ -->
<!-- batchTotal.gml                              -->
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
  <goal name="batchTotal">
    <usercontrol>
      <shrink>width</shrink>
      <backcolor>0xFF000000</backcolor>
      <children>
        <layout>
          <idname>../widgetMappings.iml#BATCH_TOTAL_SINGLE_LAYOUT</idname>
          <shrink>none</shrink>
          <expand>all</expand>
          <position>
            <x>0</x>
            <y>0</y>
            <unit>perthousand</unit>
          </position>
          <size>
            <width>1000</width>
            <height>1000</height>
            <unit>perthousand</unit>
          </size>
          <children>
            <label>
              <idname>../widgetMappings.iml#BATCH_TOTAL_HOST_NAME</idname>
              <shrink>none</shrink>
              <textalign>left</textalign>
              <position>
                <x>0</x>
                <y>200</y>
                <unit>perthousand</unit>
              </position>
              <size>
                <width>400</width>
                <height>150</height>
                <unit>perthousand</unit>
              </size>
              <text>Host</text>
            </label>
            <label>
              <idname>../widgetMappings.iml#BATCH_TOTAL_SALES_TOTAL</idname>
              <shrink>none</shrink>
              <textalign>right</textalign>
              <position>
                <x>500</x>
                <y>400</y>
                <unit>perthousand</unit>
              </position>
              <size>
                <width>500</width>
                <height>150</height>
                <unit>perthousand</unit>
              </size>
              <text>HKD 9999</text>
            </label>
            <label>
              <idname>../widgetMappings.iml#BATCH_TOTAL_REFUND_TOTAL</idname>
              <shrink>none</shrink>
              <textalign>right</textalign>
              <position>
                <x>500</x>
                <y>600</y>
                <unit>perthousand</unit>
              </position>
              <size>
                <width>500</width>
                <height>150</height>
                <unit>perthousand</unit>
              </size>
              <text>HKD 9999</text>
            </label>
            <label>
              <idname>../widgetMappings.iml#BATCH_TOTAL_GRAND_TOTAL</idname>
              <shrink>none</shrink>
              <textalign>right</textalign>
              <position>
                <x>500</x>
                <y>800</y>
                <unit>perthousand</unit>
              </position>
              <size>
                <width>500</width>
                <height>150</height>
                <unit>perthousand</unit>
              </size>
              <text>HKD 9999</text>
            </label>
            <label>
              <shrink>none</shrink>
              <textalign>left</textalign>
              <position>
                <x>0</x>
                <y>600</y>
                <unit>perthousand</unit>
              </position>
              <size>
                <width>300</width>
                <height>145</height>
                <unit>perthousand</unit>
              </size>
              <text>REFUND:</text>
            </label>
            <label>
              <shrink>none</shrink>
              <textalign>left</textalign>
              <position>
                <x>0</x>
                <y>400</y>
                <unit>perthousand</unit>
              </position>
              <size>
                <width>300</width>
                <height>150</height>
                <unit>perthousand</unit>
              </size>
              <text>SALES:</text>
            </label>
            <label>
              <shrink>none</shrink>
              <textalign>left</textalign>
              <position>
                <x>0</x>
                <y>800</y>
                <unit>perthousand</unit>
              </position>
              <size>
                <width>300</width>
                <height>150</height>
                <unit>perthousand</unit>
              </size>
              <text>TOTAL:</text>
            </label>
            <label>
              <shrink>none</shrink>
              <textalign>left</textalign>
              <position>
                <x>0</x>
                <y>0</y>
                <unit>perthousand</unit>
              </position>
              <size>
                <width>450</width>
                <height>150</height>
                <unit>perthousand</unit>
              </size>
              <text>BATCH TOTAL</text>
            </label>
          </children>
        </layout>
      </children>
    </usercontrol>
  </goal>
</DAL>