from sst.actions import *

base_url = "https://localhost:8080/"

def testuj_puste_logowanie():
    go_to(base_url)
    sleep(3)
    user = get_element(tag='input', name='user')
    passw = get_element(tag='input', name='pass')
    
    write_textfield(user,'admin', check=False)
    #write_textfield(passw,'')
    submit = get_element(tag='input', type='submit')
    click_element(submit)
    sleep(3)
    go_to(base_url)
    assert_element(tag='input', value='Logout', type='submit')

testuj_puste_logowanie()
