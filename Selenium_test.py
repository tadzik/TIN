from sst.actions import *


base_url = "https://localhost:8080/"



def zaloguj():
    go_to(base_url)
    sleep(3)
    user = get_element(tag='input', name='user')
    passw = get_element(tag='input', name='pass')
    
    write_textfield(user,'admin')
    write_textfield(passw,'dupa.8')
    submit = get_element(tag='input', type='submit')
    click_element(submit)

    
def testuj_czy_get():

    text1 = get_element(tag='input', name='id1')
    text2 = get_element(tag='input', name='id2')
    write_textfield(text1,u'ksztaltna dupa', check = False) 
    sleep(3)
    write_textfield(text2,u'ksztaltne cycki', check = False)
    sleep(3)
    submit = get_element(tag='input', type='submit')
    click_element(submit)
    sleep(3)
    go_to(base_url)         #get ?
    if assert_element(tag='input', name='id2'):
        ok=True
    sleep(3)
    go_to(base_url)
    sleep(3)
    print ok
   
zaloguj()   
testuj_czy_get()
