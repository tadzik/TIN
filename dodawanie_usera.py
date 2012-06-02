from sst.actions import *


base_url = "https://localhost:8080/"



def zaloguj():
    go_to(base_url)
    sleep(3)
    user = get_element(tag='input', name='user')
    passw = get_element(tag='input', name='pass')
    
    write_textfield(user,'admin')
    write_textfield(passw,'admin.8')
    submit = get_element(tag='input', type='submit')
    click_element(submit)
    sleep(3)
    
def dodaj_usera():
    go_to(base_url)
    sleep(3)
    link = get_element(tag='a', id='useradd')
    click_element(link)
    sleep(3)
    user = get_element(tag='input', name='user')
    passw = get_element(tag='input', name='pass')
    
    write_textfield(user,'newuser')
    write_textfield(passw,'user')
    
    submit = get_element(tag='input', type='submit')
    click_element(submit) 
    sleep(3)
    
    logout = get_element(tag='input', value='Logout', type='submit')
    click_element(logout)
    sleep(3)
    
def zaloguj_nowego_usera():
    
    sleep(3)
    user = get_element(tag='input', name='user')
    passw = get_element(tag='input', name='pass')
    
    write_textfield(user,'newuser')
    write_textfield(passw,'user')
    submit = get_element(tag='input', type='submit')
    click_element(submit)
    sleep(3)
    assert_element(tag='input', value='Logout', type='submit')
    
zaloguj()
dodaj_usera()
zaloguj_nowego_usera()
    
