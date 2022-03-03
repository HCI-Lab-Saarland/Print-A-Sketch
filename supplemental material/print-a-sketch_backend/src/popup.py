from kivy.uix.floatlayout import FloatLayout
from kivy.uix.popup import Popup


class P(FloatLayout):
    pass

def show_popup():
    show = P()
    popupWindow = Popup(title="load", content=show, size_hint=(None, None), size=(400, 400))
    popupWindow.open()
