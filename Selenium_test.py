from sst.actions import *


base_url = "https://localhost:8080/"



def zaloguj():
    pass
    
def testuj_czy_get():
    go_to(base_url)
    text1 = get_element(tag='input', name='id1')
    text2 = get_element(tag='input', name='id2')
    write_textfield(text1,u'ksztaltna dupa') 
    sleep(3)
    write_textfield(text2,u'ksztaltne cycki')
    sleep(3)
    submit = get_element(tag='input', type='submit')
    click_element(submit)
    sleep(10)
    go_to(base_url)
    sleep(10)
    go_to(base_url)
   
testuj_czy_get()
