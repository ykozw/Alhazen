# TODOs
# - イメージのサイズの変更(UI経由で)
# - 書き込み先イメージ名をいじれるように(UI経由で)
# - イメージ書き込み後に自動でリフレッシュされているかのチェック
# - Blener上のindex情報などをpydに渡せるようにする

import bpy
from bpy.props import IntProperty, FloatProperty
from bpy.props import EnumProperty, FloatVectorProperty
import bmesh
import AlhazenPy

bl_info = {
    "name": "Alhazen",
    "author" : "q",
    "version" : (1, 0),
    "blender" : (2, 75, 0),
    "location" : "",
    "description" : "Alhazeの呼び出し",
    "warning" : "",
    "support" : "TESTING",
    "wiki_url" : "",
    "tracker_url" : "",
    "category" : "User Interface"
}

def main():
    imgName = "ALHAZEN"
    imgSize = [128,128]
    images = bpy.data.images
    # 書き込み先イメージの作成
    if images.find(imgName) == -1:
        images.new(name=imgName,width=imgSize[0],height=imgSize[1]) 
    img = images[imgName] 

    # レンダラーの作成
    render = AlhazenPy.Render(imgSize[0],imgSize[1]);
    buffer = render.getBuffer();

    # レンダリング結果のコピー
    pixels = [None] * imgSize[0] * imgSize[1]
    for x in range(imgSize[0]):
        for y in range(imgSize[1]):
            pixels[(y * imgSize[0]) + x] = buffer[x+y*imgSize[0]]
    # 何かちゃんとしたfloatにしている？
    pixels = [chan for px in pixels for chan in px]
    img.pixels = pixels


class NullOperation(bpy.types.Operator):

    bl_idname = "object.null_operation"
    bl_label = "NOP"
    bl_description = "何もしない"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        return {'FINISHED'}


class RenderOperation(bpy.types.Operator):

    bl_idname = "alhazen.render"
    bl_label = "NOP"
    bl_description = "Renderereer"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        #main()
        print("EXECUTE")
        # 選択中のメッシュを得る
        bm = bmesh.new()
        me = bpy.context.object.data
        bm.from_mesh(me)

        bm.verts.ensure_lookup_table()
        bm.faces.ensure_lookup_table()
        print(dir(bm))
        print(dir(bm.verts[0]))
        print(bm.verts)
        print(bm.faces)
        # Modify the BMesh, can do anything here...
        for v in bm.verts:
            print("index: %d" % v.index)
            print("norm : %.2f %.2f %.2f" % (v.normal.x, v.normal.y, v.normal.z) )

        for face in bm.faces:
            print(face)


        #print(dir(m))
        return {'FINISHED'}


class NullOperationMenu(bpy.types.Menu):

    bl_idname = "object.null_operation_menu"
    bl_label = "NOP メニュー"
    bl_description = "何もしない処理を複数持つメニュー"

    def draw(self, context):
        layout = self.layout
        # メニュー項目の追加
        for i in range(3):
            layout.operator(NullOperation.bl_idname, text=("項目 %d" % (i)))


class ShowAllIcons(bpy.types.Operator):

    bl_idname = "object.show_all_icons"
    bl_label = "利用可能なアイコンをすべて表示"
    bl_description = "利用可能なアイコンをすべて表示"
    bl_options = {'REGISTER', 'UNDO'}

    num_column = IntProperty(
        name="一行に表示するアイコン数",
        description="一行に表示するアイコン数",
        default=2,
        min=1,
        max=5
    )

    # オプションのUI
    def draw(self, context):
        layout = self.layout

        layout.prop(self, "num_column")

        layout.separator()

        # 利用可能なアイコンをすべて表示
        layout.label(text="利用可能なアイコン一覧:")
        icon = bpy.types.UILayout.bl_rna.functions['prop'].parameters['icon']
        for i, key in enumerate(icon.enum_items.keys()):
            if i % self.num_column == 0:
                row = layout.row()
            row.label(text=key, icon=key)

    def execute(self, context):
        return {'FINISHED'}

class MySettings(bpy.types.PropertyGroup):
    prop_image_width = IntProperty(
        name="ImageWidth",
        description="ImageWidth",
        default=256,
        min=64,
        max=2048
    )
    prop_image_height = IntProperty(
        name="ImageHeight",
        description="ImageHeight",
        default=256,
        min=64,
        max=2048
    )

# ツールシェルフに「カスタムメニュー」タブを追加
class AlhazenPanel(bpy.types.Panel):
    bl_label = "Alhazen"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'TOOLS'
    bl_category = "Alhazen"
    bl_context = "objectmode"

    # メニューの描画処理
    def draw(self, context):
        layout = self.layout
        scene = context.scene
        #print(dir(scene))
        #alhazen = context.scene.alhazen

        # ボタンを追加
        layout.operator(RenderOperation.bl_idname, text="レンダリング")

        # 上下の間隔を空ける
        layout.separator()

        # 画像解像度設定
        layout.label(text="画像設定")
        #layout.prop(scene, "prop_image_width", text="幅")
        #layout.prop(scene, "prop_image_height", text="縦")

        # メニューを追加
        layout.label(text="メニューを追加する:")
        layout.menu(NullOperationMenu.bl_idname, text="メニュー")

        layout.separator()

        # プロパティを追加
        layout.label(text="プロパティを追加する:")
        #layout.prop(scene, "cm_prop_int", text="プロパティ 1")
        #layout.prop(scene, "cm_prop_float", text="プロパティ 2")
        # layout.prop(scene, "cm_prop_enum", text="プロパティ 3")
        #layout.prop(scene, "cm_prop_floatv", text="プロパティ 4")

        layout.separator()

        # 一行に並べる（アライメント無）
        layout.label(text="一行に並べる（アライメント無）:")
        row = layout.row(align=False)
        for i in range(3):
            row.operator(NullOperation.bl_idname, text=("列 %d" % (i)))

        layout.separator()

        # 一行に並べる（アライメント有）
        layout.label(text="一行に並べる（アライメント有）:")
        row = layout.row(align=True)
        for i in range(3):
            row.operator(NullOperation.bl_idname, text=("列 %d" % (i)))

        layout.separator()

        # 一列に並べる（アライメント無）
        layout.label(text="一列に並べる（アライメント無）:")
        column = layout.column(align=False)
        for i in range(3):
            column.operator(NullOperation.bl_idname, text=("行 %d" % (i)))

        layout.separator()

        # 一列に並べる（アライメント有）
        layout.label(text="一列に並べる（アライメント有）:")
        column = layout.column(align=True)
        for i in range(3):
            column.operator(NullOperation.bl_idname, text=("行 %d" % (i)))

        layout.separator()

        # 複数列に配置する
        layout.label(text="複数列に配置する:")
        column = layout.column(align=True)
        row = column.row(align=True)
        row.operator(NullOperation.bl_idname, text="列 1, 行 1")
        row.operator(NullOperation.bl_idname, text="列 2, 行 1")
        row = column.row(align=True)
        row.operator(NullOperation.bl_idname, text="列 1, 行 2")
        row.operator(NullOperation.bl_idname, text="列 2, 行 2")

        layout.separator()

        # 領域を分割する
        layout.label(text="領域を分割する:")
        split = layout.split(percentage=0.3)
        column = split.column(align=True)
        column.label(text="領域1:")
        column.operator(NullOperation.bl_idname, text="行 1")
        column.operator(NullOperation.bl_idname, text="行 2")
        split = split.split(percentage=0.7)
        column = split.column()
        column.label(text="領域2:")
        column.operator(NullOperation.bl_idname, text="行 1")
        column.operator(NullOperation.bl_idname, text="行 2")
        split = split.split(percentage=1.0)
        column = split.column(align=False)
        column.label(text="領域3:")
        column.operator(NullOperation.bl_idname, text="行 1")
        column.operator(NullOperation.bl_idname, text="行 2")

        layout.separator()

        # 横幅を自動的に拡大する
        layout.label(text="横幅を自動的に拡大する:")
        row = layout.row()
        row.alignment = 'EXPAND'
        row.operator(NullOperation.bl_idname, text="列 1")
        row.operator(NullOperation.bl_idname, text="列 2")

        layout.separator()

        # 左寄せする
        layout.label(text="左寄せする:")
        row = layout.row()
        row.alignment = 'LEFT'
        row.operator(NullOperation.bl_idname, text="列 1")
        row.operator(NullOperation.bl_idname, text="列 2")

        layout.separator()

        # 右寄せする
        layout.label(text="右寄せする:")
        row = layout.row()
        row.alignment = 'RIGHT'
        row.operator(NullOperation.bl_idname, text="列 1")
        row.operator(NullOperation.bl_idname, text="列 2")

        layout.separator()

        # グループ化する
        layout.label(text="グループ化する:")
        row = layout.row()
        box = row.box()
        box_row = box.row()
        box_column = box_row.column()
        box_column.operator(NullOperation.bl_idname, text="行 1, 列 1")
        box_column.separator()
        box_column.operator(NullOperation.bl_idname, text="行 2, 列 1")
        box_row.separator()
        box_column = box_row.column()
        box_column.operator(NullOperation.bl_idname, text="行 1, 列 2")
        box_column.separator()
        box_column.operator(NullOperation.bl_idname, text="行 2, 列 2")

        layout.separator()

        # プロパティのUIをカスタマイズする＋アイコン一覧を表示する
        layout.label(text="プロパティのUIをカスタマイズする")
        layout.operator(ShowAllIcons.bl_idname)


# プロパティの初期化
def init_props():
    scene = bpy.types.Scene
    scene.cm_prop_int = IntProperty(
        name="Prop 1",
        description="Integer Property",
        default=100,
        min=0,
        max=255
    )
    scene.cm_prop_float = FloatProperty(
        name="Prop 2",
        description="Float Property",
        default=0.75,
        min=0.0,
        max=1.0
    )


    scene.cm_prop_enum = EnumProperty(
        name="Prop 3",
        description="Enum Property",
        items=[
            ('ITEM_1', "項目 1", "項目 1"),
            ('ITEM_2', "項目 2", "項目 2"),
            ('ITEM_3', "項目 3", "項目 3")
        ],
        default='ITEM_1'
    )
    scene.cm_prop_floatv = FloatVectorProperty(
        name="Prop 4",
        description="Float Vector Property",
        subtype='LAYER',
        default=(1.0, 1.0, 1.0),
        min=0.0,
        max=1.0
    )

    scene.prop_image_width = IntProperty(
        name="ImageWidth",
        description="ImageWidth",
        default=256,
        min=64,
        max=2048
    )

    scene.prop_image_height = IntProperty(
        name="ImageHeight",
        description="ImageHeight",
        default=256,
        min=64,
        max=2048
    )
    #scene.cm_prop_unko = PointerProperty(type=MySettings)


# プロパティを削除
def clear_props():
    scene = bpy.types.Scene
    del scene.cm_prop_int
    del scene.cm_prop_float
    del scene.cm_prop_enum
    del scene.cm_prop_floatv


def register():
    bpy.utils.register_module(__name__)
    #bpy.utils.register_class(MySettings)
    init_props()


def unregister():
    bpy.utils.unregister_module(__name__)
    bpy.utils.unregister_module(MySettings)
    clear_props()


if __name__ == "__main__":
    register()