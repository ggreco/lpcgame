include_directories("${PROJECT_SOURCE_DIR}/tinyxml")
add_library(tmxparser TmxImage.cpp   TmxMap.cpp          TmxObjectGroup.cpp  TmxPolyline.cpp     
                      TmxTile.cpp    TmxUtil.cpp         TmxLayer.cpp        TmxObject.cpp 
                      TmxPolygon.cpp TmxPropertySet.cpp  TmxTileset.cpp      base64)
