import flask
import logging
import psycopg2
from flask import request, jsonify
from datetime import datetime
import jwt


app = flask.Flask(__name__)

StatusCodes = {
    'success': 200,
    'api_error': 400,
    'internal_error': 500
}

##########################################################
## DATABASE ACCESS
##########################################################

def db_connection():
    try:
        db = psycopg2.connect(
            user='aulaspl',
            password='aulaspl',
            host='127.0.0.1',
            port='5432',
            database='projeto'
        )
        return db
    except (Exception, psycopg2.DatabaseError) as error:
        logging.error(f"Error connecting to the database: {error}")
        return None

##########################################################
## ENDPOINTS
##########################################################

@app.route('/')
def landing_page():
    return """
    Hello World (Python Native)!  <br/>
    <br/>
    Check the sources for instructions on how to use the endpoints!<br/>
    <br/>
    BD 2023-2024 Team<br/>
    <br/>
    """

#Registar user dependendo do seu cargo
def register_user(user_data):
    conn = db_connection()
    if not conn:
        return {'status': StatusCodes['internal_error'], 'errors': 'Database connection failed'}

    cur = conn.cursor()

    try:
        # Insert into user_data table
        cur.execute('INSERT INTO user_data (username, password, address, phone_number) VALUES (%s, %s, %s, %s) RETURNING id_user', 
                    (user_data['username'], user_data['password'], user_data['address'], user_data['phone_number']))
        user_id = cur.fetchone()[0]

        # Insert into employees table if necessary
        if user_data['role'] in ['doctor', 'nurse', 'assistant']:
            cur.execute('INSERT INTO employees (user_data_id_user) VALUES (%s)', (user_id,))

        # Insert into the specific role table
        if user_data['role'] == 'patient':
            cur.execute('INSERT INTO patient (user_data_id_user) VALUES (%s)', (user_id,))
        elif user_data['role'] == 'doctor':
            cur.execute('INSERT INTO doctor (licenseid, employees_user_data_id_user) VALUES (%s, %s)', 
                        (user_data['licenseid'], user_id))

            # Insert specialties if not exist
            for spec in user_data.get('specializations', []):
                cur.execute('INSERT INTO specialization (specname) SELECT %s WHERE NOT EXISTS (SELECT 1 FROM specialization WHERE specname = %s)', 
                            (spec, spec))

                # Link doctor to specialization
                cur.execute('INSERT INTO doctor_specialization (doctor_employees_user_data_id_user, specialization_specname) VALUES (%s, %s)', 
                            (user_id, spec))
                
        elif user_data['role'] == 'nurse':
            # Check if the category exists, and if not, insert it
            category_name = user_data['category_name']
            cur.execute('INSERT INTO category (name) SELECT %s WHERE NOT EXISTS (SELECT 1 FROM category WHERE name = %s)', 
                        (category_name, category_name))
            
            # Get category id
            cur.execute('SELECT categoryid FROM category WHERE name = %s', (category_name,))
            category_id = cur.fetchone()[0]
            
            cur.execute('INSERT INTO nurses (category_categoryid, employees_user_data_id_user) VALUES (%s, %s)', 
                        (category_id, user_id))
        elif user_data['role'] == 'assistant':
            cur.execute('INSERT INTO assistants (employees_user_data_id_user) VALUES (%s)', 
                        (user_id,))
        
        conn.commit()
        response = {'status': StatusCodes['success'], 'results': user_id}

    except (Exception, psycopg2.DatabaseError) as error:
        conn.rollback()
        response = {'status': StatusCodes['internal_error'], 'errors': str(error)}

    finally:
        if conn is not None:
            conn.close()

    return response


@app.route('/dbproj/register/<role>', methods=['POST'])
def register_user_endpoint(role):
    payload = request.get_json()
    common_fields = ['username', 'password', 'address', 'phone_number']
    role_fields = {
        'patient': [],
        'doctor': ['licenseid', 'specializations'],
        'nurse': ['category_name'],  # Alterado para incluir category_name
        'assistant': []
    }

    if role not in role_fields:
        return jsonify({'status': StatusCodes['api_error'], 'results': f'Invalid role: {role}'})

    required_fields = common_fields + role_fields[role]
    missing_fields = [field for field in required_fields if field not in payload]

    if missing_fields:
        return jsonify({'status': StatusCodes['api_error'], 'results': f'Missing fields: {", ".join(missing_fields)}'})

    payload['role'] = role
    response = register_user(payload)
        
    return jsonify(response)


# Autenticação
def authenticate_user(username, password):
    conn = db_connection()
    if not conn:
        return {'status': StatusCodes['internal_error'], 'errors': 'Database connection failed'}

    cur = conn.cursor()
    try:
        # Verifica se o usuário e senha estão corretos
        cur.execute('SELECT id_user FROM user_data WHERE username = %s AND password = %s', (username, password))
        user_data = cur.fetchone()

        if not user_data:
            return {'status': StatusCodes['api_error'], 'errors': 'Invalid username or password'}

        user_id = user_data

        # Gera token com o ID do usuário
        auth_token = jwt.encode({'user_id': user_id}, 'chave_descodificacao', algorithm='HS256')

        return {'status': StatusCodes['success'], 'results': auth_token}

    except (Exception, psycopg2.DatabaseError) as error:
        conn.rollback()
        return {'status': StatusCodes['internal_error'], 'errors': str(error)}
    finally:
        if conn is not None:
            conn.close()

# Endpoint para autenticar usuário e gerar token JWT
@app.route('/dbproj/user', methods=['PUT'])
def authenticate_user_endpoint():
    payload = request.get_json()
    if 'username' not in payload or 'password' not in payload:
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Missing username or password'}), StatusCodes['api_error']

    username = payload['username']
    password = payload['password']

    response = authenticate_user(username, password)
    if response['status'] == StatusCodes['success']:
        return jsonify(response), StatusCodes['success']
    else:
        return jsonify(response), response['status']
def is_patient(user_id):
    conn = db_connection()
    if not conn:
        return False
    cur = conn.cursor()
    try:
        cur.execute('SELECT user_data_id_user FROM patient WHERE user_data_id_user = %s', (user_id,))
        patient_id = cur.fetchone()
        if patient_id:
            return True
        else:
            return False
    except:
        return False
    finally:
        if conn:
            conn.close()

def is_assistant(user_id):
    conn = db_connection()
    if not conn:
        return False
    cur = conn.cursor()
    try:
        cur.execute('SELECT 1 FROM assistants WHERE employees_user_data_id_user = %s', (user_id,))
        is_assistant = cur.fetchone()
        return is_assistant is not None
    except:
        return False
    finally:
        if conn:
            conn.close()


def is_doctor(user_id):
    conn = db_connection()
    if not conn:
        return False
    cur = conn.cursor()
    try:
        cur.execute('SELECT 1 FROM doctor WHERE employees_user_data_id_user = %s', (user_id,))
        is_doctor = cur.fetchone()
        return is_doctor is not None
    except:
        return False
    finally:
        if conn:
            conn.close()

def is_employee_or_targeted_patient(user_id, patient_user_id):
    conn = db_connection()
    if not conn:
        return False
    
    cur = conn.cursor()
    try:
        # Verifica se o usuário é um empregado (admin, doctor, nurse)
        cur.execute('SELECT 1 FROM employees WHERE user_data_id_user = %s', (user_id,))
        is_employee = cur.fetchone()

        # Verifica se o usuário é o paciente alvo
        cur.execute('SELECT 1 FROM patient WHERE user_data_id_user = %s', (patient_user_id,))
        is_targeted_patient = cur.fetchone()
        if is_employee or (is_targeted_patient and patient_user_id == user_id):
            return True
        else:
            return False
    except:
        return False
    finally:
        if conn:
            conn.close()


def decode_auth_token(auth_token):
    try:
        payload = jwt.decode(auth_token, 'chave_descodificacao', algorithms=['HS256'])
        user_id = payload.get('user_id')
        if isinstance(user_id, list):
            # Se for uma lista, retornar o primeiro elemento
            return user_id[0]
        else:
            return user_id
    except jwt.ExpiredSignatureError:
        return None  # Token expirado
    except jwt.InvalidTokenError:
        return None  # Token inválido
#Marcação de appointment
def schedule_appointment(patient_id, doctor_id, date, nurse_ids=None):
    conn = db_connection()
    if not conn:
        return {'status': StatusCodes['internal_error'], 'errors': 'Database connection failed'}

    cur = conn.cursor()
    try:
        # Check if the doctor with the given ID exists
        cur.execute('SELECT COUNT(*) FROM doctor WHERE employees_user_data_id_user = %s', (doctor_id,))
        doctor_exists = cur.fetchone()[0]

        if not doctor_exists:
            return {'status': StatusCodes['api_error'], 'errors': f'Doctor with ID {doctor_id} does not exist'}

        # Insert a new appointment
        cur.execute('INSERT INTO appointments (date_app, doctor_employees_user_data_id_user, patient_user_data_id_user) VALUES (%s, %s, %s) RETURNING appointmentid', 
                    (date, doctor_id, patient_id))
        appointment_id = cur.fetchone()[0]

        # Insert nurses into appointments_nurses if nurse_ids is provided
        if nurse_ids:
            for nurse_id in nurse_ids:
                cur.execute('INSERT INTO appointments_nurses (appointments_appointmentid, nurses_employees_user_data_id_user) VALUES (%s, %s)', 
                            (appointment_id, nurse_id))
        
        conn.commit()
        response = {'status': StatusCodes['success'], 'results': appointment_id}

    except (Exception, psycopg2.DatabaseError) as error:
        conn.rollback()
        response = {'status': StatusCodes['internal_error'], 'errors': str(error)}

    finally:
        if conn is not None:
            conn.close()

    return response


# Endpoint para agendar um appointment
@app.route('/dbproj/appointment', methods=['POST'])
def schedule_appointment_endpoint():
    payload = request.get_json()
    if 'doctor_id' not in payload or 'date' not in payload:
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Missing doctor_id or date'})

    doctor_id = payload['doctor_id']
    date = payload['date']
    nurse_ids = payload.get('nurse_ids', None)

    # Recupera o token JWT do cabeçalho de autenticação
    auth_header = request.headers.get('Authorization')
    if not auth_header:
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Authorization token is missing'})
    
    if not auth_header.startswith('Bearer '):
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Invalid authorization token format'})
    auth_token = auth_header.split(" ")[1]
    
    # Decodifica o token JWT para obter o ID do paciente
    patient_id = decode_auth_token(auth_token)
    if not patient_id or not is_patient(patient_id):
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'User is not authorized to schedule appointments'})

    # Agendar a consulta
    response = schedule_appointment(patient_id, doctor_id, date, nurse_ids)
    return jsonify(response)

def get_patient_appointments(patient_id):
    conn = db_connection()
    if not conn:
        return {'status': StatusCodes['internal_error'], 'errors': 'Database connection failed'}

    cur = conn.cursor()
    try:
        # Consulta para obter todas as consultas do paciente
        cur.execute('SELECT appointmentid, doctor_employees_user_data_id_user, date_app FROM appointments WHERE patient_user_data_id_user = %s', (patient_id,))
        appointments = cur.fetchall()

        # Lista para armazenar os resultados
        results = []
        for appointment in appointments:
            appointment_id, doctor_id, date = appointment

            # Obtém o nome do médico usando o ID do médico
            cur.execute('SELECT username FROM user_data WHERE id_user = %s', (doctor_id,))
            doctor_name = cur.fetchone()[0]

            # Obtém os enfermeiros associados a esta consulta
            cur.execute('SELECT nurses_employees_user_data_id_user FROM appointments_nurses WHERE appointments_appointmentid = %s', (appointment_id,))
            nurse_ids = cur.fetchall()

            # Lista para armazenar os nomes dos enfermeiros
            nurses = []
            for nurse_id in nurse_ids:
                cur.execute('SELECT username FROM user_data WHERE id_user = %s', (nurse_id,))
                nurse_name = cur.fetchone()[0]
                nurses.append({'nurse_id': nurse_id[0], 'nurse_name': nurse_name})

            # Adiciona os detalhes da consulta à lista de resultados
            results.append({'id': appointment_id, 'doctor_id': doctor_id, 'doctor_name': doctor_name, 'date': date, 'nurses': nurses})

        return {'status': StatusCodes['success'], 'results': results}

    except (Exception, psycopg2.DatabaseError) as error:
        return {'status': StatusCodes['internal_error'], 'errors': str(error)}

    finally:
        if conn is not None:
            conn.close()

@app.route('/dbproj/appointments/<int:patient_user_id>', methods=['GET'])
def get_patient_appointments_endpoint(patient_user_id):
    # Recupera o token JWT do cabeçalho de autenticação
    auth_header = request.headers.get('Authorization')
    if not auth_header:
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Authorization token is missing'})

    if not auth_header.startswith('Bearer '):
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Invalid authorization token format'})
    auth_token = auth_header.split(" ")[1]

    # Decodifica o token JWT para obter o ID do usuário autenticado
    decoded_user_id = decode_auth_token(auth_token)
    if not decoded_user_id:
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Invalid authorization token'})

    # Verifica se o usuário autenticado é um funcionário ou o próprio paciente alvo
    if not is_employee_or_targeted_patient(decoded_user_id, patient_user_id):
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Unauthorized access'})

    # Recupera as consultas do paciente
    response = get_patient_appointments(patient_user_id)
    return jsonify(response)


def schedule_surgery(patient_id, doctor_id, nurses, date, surgery_type='surgery', hospitalization_id=None, responsiblenurse=None, date_start=None, date_end=None, nurses_employees_user_data_id_user=None, assistants_employees_user_data_id_user=None):
    conn = db_connection()
    if not conn:
        return {'status': StatusCodes['internal_error'], 'errors': 'Database connection failed'}

    cur = conn.cursor()
    try:
        # Verificar se o médico com o ID fornecido existe
        cur.execute('SELECT COUNT(*) FROM doctor WHERE employees_user_data_id_user = %s', (doctor_id,))
        doctor_exists = cur.fetchone()[0]
        if not doctor_exists:
            return {'status': StatusCodes['api_error'], 'errors': f'Doctor with ID {doctor_id} does not exist'}

        # Inserir a nova hospitalização, se hospitalization_id não for fornecido
        if hospitalization_id is None:
            cur.execute('INSERT INTO hospitalization (responsiblenurse,date_start, date_end,nurses_employees_user_data_id_user,patient_user_data_id_user, assistants_employees_user_data_id_user) VALUES (%s, %s, %s, %s, %s, %s) RETURNING hospid', 
                        (responsiblenurse,date_start, date_end, nurses_employees_user_data_id_user,patient_id,assistants_employees_user_data_id_user))
            hospitalization_id = cur.fetchone()[0]

        # Inserir a nova cirurgia
        cur.execute('INSERT INTO surgery (type, date_surgery, doctor_employees_user_data_id_user,hospitalization_hospid) VALUES (%s, %s, %s, %s) RETURNING surgeryid', 
                    (surgery_type, date, doctor_id, hospitalization_id))
        surgery_id = cur.fetchone()[0]

        # Associar enfermeiros à cirurgia
        for nurse_id in nurses:
            cur.execute('INSERT INTO surgery_nurses (surgery_surgeryid, nurses_employees_user_data_id_user) VALUES (%s, %s)', 
                        (surgery_id, nurse_id))

        conn.commit()
        response = {
            'status': StatusCodes['success'],
            'results': {
                'hospitalization_id': hospitalization_id,
                'surgery_id': surgery_id,
                'patient_id': patient_id,
                'doctor_id': doctor_id,
                'date': date,
                'type': surgery_type
            }
        }

    except (Exception, psycopg2.DatabaseError) as error:
        conn.rollback()
        response = {'status': StatusCodes['internal_error'], 'errors': str(error)}

    finally:
        if conn is not None:
            conn.close()

    return response



# Endpoint para agendar uma cirurgia
@app.route('/dbproj/surgery/', methods=['POST'])
def schedule_surgery_endpoint():
    payload = request.get_json()
    # Verificar se o assistente está autenticado
    auth_header = request.headers.get('Authorization')
    if not auth_header:
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Authorization token is missing'})
    
    if not auth_header.startswith('Bearer '):
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Invalid authorization token format'})
    
    auth_token = auth_header.split(" ")[1]
    user_id = decode_auth_token(auth_token)
    if not user_id or not is_assistant(user_id):
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'User is not authorized to schedule surgeries'})
    required_fields = ['patient_id', 'doctor', 'nurses', 'date', 'type', 'responsiblenurse', 'date_start', 'date_end', 'nurses_employees_user_data_id_user', 'assistants_employees_user_data_id_user']
    missing_fields = [field for field in required_fields if field not in payload]
    if missing_fields:
        return jsonify({'status': StatusCodes['api_error'], 'errors': f'Missing fields: {", ".join(missing_fields)}'})

    patient_id = payload['patient_id']
    doctor_id = payload['doctor']
    nurses = payload['nurses']
    date = payload['date']
    surgery_type = payload['type']
    responsiblenurse = payload['responsiblenurse']
    date_start = payload['date_start']
    date_end = payload['date_end']
    nurses_employees_user_data_id_user = payload['nurses_employees_user_data_id_user']
    assistants_employees_user_data_id_user = payload['assistants_employees_user_data_id_user']

    response = schedule_surgery(patient_id, doctor_id, nurses, date, surgery_type,None, responsiblenurse, date_start, date_end, nurses_employees_user_data_id_user, assistants_employees_user_data_id_user)
    return jsonify(response)


# Endpoint para agendar uma cirurgia associada a uma hospitalização existente
@app.route('/dbproj/surgery/<int:hospitalization_id>', methods=['POST'])
def schedule_surgery_with_hospitalization_endpoint(hospitalization_id):
    payload = request.get_json()
    # Verificar se o assistente está autenticado
    auth_header = request.headers.get('Authorization')
    if not auth_header:
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Authorization token is missing'})
    
    if not auth_header.startswith('Bearer '):
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Invalid authorization token format'})
    
    auth_token = auth_header.split(" ")[1]
    user_id = decode_auth_token(auth_token)
    if not user_id or not is_assistant(user_id):
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'User is not authorized to schedule surgeries'})
    required_fields = ['patient_id', 'doctor', 'nurses', 'date', 'type']
    missing_fields = [field for field in required_fields if field not in payload]
    if missing_fields:
        return jsonify({'status': StatusCodes['api_error'], 'errors': f'Missing fields: {", ".join(missing_fields)}'})

    patient_id = payload['patient_id']
    doctor_id = payload['doctor']
    nurses = payload['nurses']
    date = payload['date']
    surgery_type = payload['type']

    response = schedule_surgery(patient_id, doctor_id, nurses, date, surgery_type, hospitalization_id)
    return jsonify(response)

def add_prescription(evento, event_id,validity,medicines):
    conn = db_connection()
    if not conn:
        return {'status': StatusCodes['internal_error'], 'errors': 'Database connection failed'}
    
    cur = conn.cursor()
    if evento == "appointment":
        try:
            # Verifica se o ID do appointment existe
            cur.execute('SELECT 1 FROM appointments WHERE appointmentid = %s', (event_id,))
            appointment_exists = cur.fetchone()
            if not appointment_exists:
                return {'status': StatusCodes['api_error'], 'errors': f'Appointment with ID {event_id} does not exist'}
            
            # Insere uma nova prescrição na tabela prescriptions e recupera o ID gerado
            cur.execute('''INSERT INTO prescriptions (date_prescription,validity) VALUES (NOW(),%s) RETURNING presciptionsid''', (validity,))
            new_prescription_id = cur.fetchone()[0]
            
            cur.execute('''INSERT INTO appointments_prescriptions (appointments_appointmentid,prescriptions_presciptionsid) VALUES (%s, %s)''', (event_id, new_prescription_id))
            for medicine in medicines:
                medicine_name = medicine['medicine']
                posology_dose = medicine['posology_dose']
                posology_frequency = medicine['posology_frequency']
                side_effects = medicine.get('side_effects', [])

                
                # Recupera o ID do medicamento ou insere um novo medicamento se não existir
                cur.execute('SELECT medicineid FROM medicine WHERE name = %s', (medicine_name,))
                medicine_id = cur.fetchone()
                if not medicine_id:
                    cur.execute('''INSERT INTO medicine (name) VALUES (%s) RETURNING medicineid''', (medicine_name,))
                    medicine_id = cur.fetchone()[0]
                else:
                    medicine_id = medicine_id[0]


                
                # Insere na tabela dose
                cur.execute('''INSERT INTO dose (amount, duration, prescriptions_presciptionsid, medicine_medicineid) VALUES (%s, %s, %s, %s)''', (posology_dose, posology_frequency, new_prescription_id, medicine_id))

                for side_effect in side_effects:
                    effect_name = side_effect['name']
                    probability = side_effect['probability']
                    severity = side_effect['severity']
                    
                    # Verifica se o efeito secundário já existe no banco de dados
                    cur.execute('SELECT effectid FROM sideeffects WHERE nome = %s', (effect_name,))
                    side_effect_id = cur.fetchone()
                    if not side_effect_id:
                        # Se não existir, insere o novo efeito secundário
                        cur.execute('''INSERT INTO sideeffects (nome) VALUES (%s) RETURNING effectid''', (effect_name,))
                        side_effect_id = cur.fetchone()[0]
                    else:
                        side_effect_id = side_effect_id[0]
                    
                    # Insere a gravidade do efeito secundário para o medicamento atual
                    cur.execute('''INSERT INTO gravity (probability, severity,sideeffects_effectid, medicine_medicineid)  VALUES (%s, %s,%s,%s)''', (probability, severity ,side_effect_id ,medicine_id))
            conn.commit()
            response = {'status': StatusCodes['success'], 'results': event_id}
        except (Exception, psycopg2.DatabaseError) as error:
            conn.rollback()
            response = {'status': StatusCodes['internal_error'], 'errors': str(error)}               
            return response
    
    elif evento == "hospitalization":
        try:
            # Verifica se o ID da hospitalização existe
            cur.execute('SELECT 1 FROM hospitalization WHERE hospid = %s', (event_id,))
            hospitalization_exists = cur.fetchone()
            if not hospitalization_exists:
                return {'status': StatusCodes['api_error'], 'errors': f'Hospitalization with ID {event_id} does not exist'}
            

            # Insere uma nova prescrição na tabela prescriptions e recupera o ID gerado
            cur.execute('''INSERT INTO prescriptions (date_prescription,validity) VALUES (NOW(), %s) RETURNING presciptionsid''', (validity,))
            new_prescription_id = cur.fetchone()[0]

            cur.execute('''INSERT INTO hospitalization_prescriptions (hospitalization_hospid,prescriptions_presciptionsid) VALUES (%s, %s)''', (event_id, new_prescription_id))

            for medicine in medicines:
                medicine_name = medicine['medicine']
                posology_dose = medicine['posology_dose']
                posology_frequency = medicine['posology_frequency']
                side_effects = medicine.get('side_effects', [])

                
                # Recupera o ID do medicamento ou insere um novo medicamento se não existir
                cur.execute('SELECT medicineid FROM medicine WHERE name = %s', (medicine_name,))
                medicine_id = cur.fetchone()
                if not medicine_id:
                    cur.execute('''INSERT INTO medicine (name) VALUES (%s) RETURNING medicineid''', (medicine_name,))
                    medicine_id = cur.fetchone()[0]
                else:
                    medicine_id = medicine_id[0]
                
                # Insere na tabela dose
                cur.execute('''INSERT INTO dose (amount, duration, prescriptions_presciptionsid, medicine_medicineid) VALUES (%s, %s, %s, %s)''', (posology_dose, posology_frequency, new_prescription_id, medicine_id))
                for side_effect in side_effects:
                    effect_name = side_effect['name']
                    probability = side_effect['probability']
                    severity = side_effect['severity']
                    
                    # Verifica se o efeito secundário já existe no banco de dados
                    cur.execute('SELECT effectid FROM sideeffects WHERE nome = %s', (effect_name,))
                    side_effect_id = cur.fetchone()
                    if not side_effect_id:
                        # Se não existir, insere o novo efeito secundário
                        cur.execute('''INSERT INTO sideeffects (nome) VALUES (%s) RETURNING effectid''', (effect_name,))
                        side_effect_id = cur.fetchone()[0]
                    else:
                        side_effect_id = side_effect_id[0]
                    
                    # Insere a gravidade do efeito secundário para o medicamento atual
                    cur.execute('''INSERT INTO gravity (probability, severity,sideeffects_effectid, medicine_medicineid)  VALUES (%s, %s,%s,%s)''', (probability, severity ,side_effect_id ,medicine_id))

            conn.commit()
            response = {'status': StatusCodes['success'], 'results': event_id}

        except (Exception, psycopg2.DatabaseError) as error:
            conn.rollback()
            response = {'status': StatusCodes['internal_error'], 'errors': str(error)}               
            return response
    
    else:
        response = {'status': StatusCodes['api_error'], 'errors': 'Invalid event type'}
            
    return response

    

@app.route('/dbproj/prescription', methods=['POST'])
def add_prescription_endpoint():
    # Extrair os dados da solicitação JSON
    data = request.json
    evento = data.get('evento')
    event_id = data.get('event_id')
    validity = data.get('validity')
    medicines = data.get('medicines')

    # Verificar se o usuário é um médico
    auth_token = request.headers.get('Authorization')
    if not auth_token:
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Authorization token is missing'})

    if not auth_token.startswith('Bearer '):
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Invalid authorization token format'})
    token = auth_token.split(" ")[1]

    decoded_user_id = decode_auth_token(token)
    if not decoded_user_id:
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Invalid authorization token'})

    if not is_doctor(decoded_user_id):
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'User is not authorized'})

    # Chamar a função add_prescription com os dados fornecidos
    response = add_prescription(evento, event_id, validity, medicines)

    # Retornar a resposta como JSON
    return jsonify(response)

def get_prescriptions(patient_id):
    conn = db_connection()
    if not conn:
        return {'status': StatusCodes['internal_error'], 'errors': 'Database connection failed'}
    
    cur = conn.cursor()
    try:
        # Consulta para obter todas as prescrições do paciente
        cur.execute('''SELECT prescriptions.presciptionsid, prescriptions.date_prescription, prescriptions.validity, medicine.name AS medicine_name,
                       dose.amount, dose.duration , 'appointment' AS source
                       FROM prescriptions
                       INNER JOIN dose ON prescriptions.presciptionsid = dose.prescriptions_presciptionsid
                       INNER JOIN medicine ON dose.medicine_medicineid = medicine.medicineid
                       INNER JOIN appointments_prescriptions ON prescriptions.presciptionsid = appointments_prescriptions.prescriptions_presciptionsid
                       INNER JOIN appointments ON appointments_prescriptions.appointments_appointmentid = appointments.appointmentid
                       WHERE appointments.patient_user_data_id_user = %s 
                        UNION ALL
                        SELECT prescriptions.presciptionsid, prescriptions.date_prescription, prescriptions.validity, medicine.name AS medicine_name,
                            dose.amount, dose.duration, 'hospitalization' AS source
                        FROM prescriptions
                        INNER JOIN dose ON prescriptions.presciptionsid = dose.prescriptions_presciptionsid
                        INNER JOIN medicine ON dose.medicine_medicineid = medicine.medicineid
                        INNER JOIN hospitalization_prescriptions ON prescriptions.presciptionsid = hospitalization_prescriptions.prescriptions_presciptionsid
                        INNER JOIN hospitalization ON hospitalization_prescriptions.hospitalization_hospid = hospitalization.hospid
                        WHERE hospitalization.patient_user_data_id_user = %s ''', (patient_id, patient_id))
                   
        prescriptions = cur.fetchall()

        # Lista para armazenar os resultados
        results = []
        for prescription in prescriptions:
            prescription_id, prescription_date, validity, medicine_name, amount, duration , source = prescription
            validity_without = validity.strftime("%Y-%m-%d")
            results.append({
                'prescription_id': prescription_id,
                'prescription_date': prescription_date.strftime('%Y-%m-%d %H:%M:%S'),
                'validity': validity_without,
                'medicine_name': medicine_name,
                'amount': amount,
                'duration': duration,
                'source': source
            })

        return {'status': StatusCodes['success'], 'prescriptions': results}

    except (Exception, psycopg2.DatabaseError) as error:
        return {'status': StatusCodes['internal_error'], 'errors': str(error)}

    finally:
        if conn is not None:
            conn.close()


@app.route('/dbproj/prescriptions/<int:patient_user_id>', methods=['GET'])
def get_prescriptions_endpoint(patient_user_id):
    # Recupera o token JWT do cabeçalho de autenticação
    auth_header = request.headers.get('Authorization')
    if not auth_header:
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Authorization token is missing'})

    if not auth_header.startswith('Bearer '):
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Invalid authorization token format'})
    auth_token = auth_header.split(" ")[1]

    # Decodifica o token JWT para obter o ID do usuário
    decoded_user_id = decode_auth_token(auth_token)
    if not decoded_user_id:
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Invalid authorization token'})

    # Verifica se o usuário é um empregado ou o paciente alvo
    if not is_employee_or_targeted_patient(decoded_user_id, patient_user_id):
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Unauthorized access'})

    # Recupera as prescrições do paciente
    response = get_prescriptions(patient_user_id)
    return jsonify(response)

# Função para obter o nome do médico com base no ID
def get_doctor_name(doctor_id):
    conn = db_connection()
    if isinstance(conn, dict):
        return None  # Em caso de erro na conexão, retorna None

    cur = conn.cursor()
    try:
        # Constrói a query para obter o nome do médico
        query = "SELECT username FROM user_data WHERE id_user = %s"
        cur.execute(query, (doctor_id,))
        doctor_data = cur.fetchone()

        # Se o médico existir no banco de dados, retorna o nome
        if doctor_data:
            return doctor_data[0]
        else:
            return None  # Retorna None se o médico não for encontrado
    except (Exception, psycopg2.DatabaseError) as error:
        # Em caso de erro, imprime o erro e retorna None
        print(f"Error fetching doctor name: {error}")
        return None
    finally:
        # Fecha a conexão com o banco de dados
        if conn is not None:
            conn.close()


@app.route('/dbproj/report', methods=['GET'])
def monthly_report():
    # Verifica se o usuário é um assistente
    auth_header = request.headers.get('Authorization')
    if not auth_header or not auth_header.startswith('Bearer '):
        return jsonify({'status': 'api_error', 'errors': 'Invalid authorization token format'})

    # Decodifica o token JWT para obter o ID do usuário
    user_id = decode_auth_token(auth_header.split(" ")[1])
    if not user_id or not is_assistant(user_id):
        return jsonify({'status': 'api_error', 'errors': 'User is not authorized to access this endpoint'})

    conn = db_connection()
    if isinstance(conn, dict):
        return conn  # Se houver um erro na conexão, retorna a resposta de erro

    cur = conn.cursor()
    try:
        # Constrói a query para obter o relatório mensal
        query = """
            SELECT EXTRACT(YEAR FROM date_surgery) as year, EXTRACT(MONTH FROM date_surgery) as month, 
            COUNT(*) as total_surgeries, doctor_employees_user_data_id_user
            FROM surgery
            WHERE date_surgery > CURRENT_DATE - INTERVAL '1 year'
            GROUP BY year, month, doctor_employees_user_data_id_user
            ORDER BY year, month, total_surgeries DESC
            LIMIT 12
        """
        cur.execute(query)
        report_data = cur.fetchall()

        # Processa os dados para formatá-los de acordo com a resposta desejada
        results = []
        for row in report_data:
            year, month, total_surgeries, doctor_id = row
            doctor_name = get_doctor_name(doctor_id)  # Função para obter o nome do médico
            results.append({'month': f'month_{int(month)}', 'doctor': doctor_name, 'surgeries': total_surgeries})

        # Retorna os resultados no formato JSON
        return jsonify({'status': 'success', 'results': results})

    except (Exception, psycopg2.DatabaseError) as error:
        # Em caso de erro, retorna uma mensagem de erro
        return jsonify({'status': 'internal_error', 'errors': str(error)})
    finally:
        # Fecha a conexão com o banco de dados
        if conn is not None:
            conn.close()

def find_bill_for_appointment(patient_id):
    conn = db_connection()
    if not conn:
        return None

    cur = conn.cursor()
    try:
        # Consulta para encontrar a fatura associada à consulta do paciente
        cur.execute('''SELECT b.billid 
                       FROM bill b
                       JOIN appointments_bill ab ON b.billid = ab.bill_billid
                       JOIN appointments a ON ab.appointments_appointmentid = a.appointmentid
                       WHERE a.patient_user_data_id_user = %s''', (patient_id,))
        appointment_bill = cur.fetchone()

        if appointment_bill:
            return appointment_bill[0]
        else:
            return None  # Se não encontrar uma fatura associada à consulta do paciente

    except (Exception, psycopg2.DatabaseError) as error:
        print("Error:", error)
        return None
    finally:
        if conn is not None:
            conn.close()

def find_bill_for_surgery(patient_id):
    conn = db_connection()
    if not conn:
        return None

    cur = conn.cursor()
    try:
        # Consulta para encontrar a fatura associada à cirurgia do paciente
        cur.execute('''SELECT b.billid 
                       FROM bill b
                       JOIN bill_hospitalization bh ON b.billid = bh.bill_billid
                       JOIN hospitalization h ON bh.hospitalization_hospid = h.hospid
                       WHERE h.patient_user_data_id_user = %s''', (patient_id,))
        surgery_bill = cur.fetchone()

        if surgery_bill:
            return surgery_bill[0]
        else:
            return None  # Se não encontrar uma fatura associada à cirurgia do paciente

    except (Exception, psycopg2.DatabaseError) as error:
        print("Error:", error)
        return None
    finally:
        if conn is not None:
            conn.close()
def execute_payment(bill_id, amount, payment_method, patient_id,date_pay, bill_type):
    conn = db_connection()
    if not conn:
        return {'status': StatusCodes['internal_error'], 'errors': 'Database connection failed'}

    cur = conn.cursor()
    try:
        if bill_type == 'appointment':
            patient_bill_id = find_bill_for_appointment(patient_id)
        elif bill_type == 'surgery':
            patient_bill_id = find_bill_for_surgery(patient_id)
        else:
            return {'status': StatusCodes['api_error'], 'errors': 'Invalid bill type'}

        if patient_bill_id != bill_id:
            return {'status': StatusCodes['api_error'], 'errors': 'Bill not found or does not belong to the patient'}
        # Obtém o valor devido atual
        cur.execute('SELECT cost FROM bill WHERE billid = %s', (bill_id,))
        amount_due = cur.fetchone()[0]

        remaining_amount = amount_due - amount

        if remaining_amount < 0:
            return {'status': StatusCodes['api_error'], 'errors': 'Payment amount exceeds the due amount'}

        # Insere o pagamento na tabela de pagamentos
        cur.execute('''INSERT INTO payment (amount, method, payment_date, bill_billid)
                       VALUES (%s, %s, %s, %s)''', (amount, payment_method,date_pay, bill_id))

        # Atualiza o valor devido na fatura
        cur.execute('UPDATE bill SET cost = %s WHERE billid = %s', (remaining_amount, bill_id))

        # Se o valor restante for zero, atualiza o status da fatura para "paid"
        if remaining_amount == 0:
            cur.execute('UPDATE bill SET payed = %s WHERE billid = %s', ("TRUE", bill_id))

        conn.commit()
        response = {'status': StatusCodes['success'], 'results': remaining_amount}

    except (Exception, psycopg2.DatabaseError) as error:
        conn.rollback()
        response = {'status': StatusCodes['internal_error'], 'errors': str(error)}
    finally:
        if conn:
            conn.close()

    return response

@app.route('/dbproj/bills/<int:bill_id>', methods=['POST'])
def pay_bill_endpoint(bill_id):
    data = request.json
    amount = data.get('amount')
    payment_method = data.get('payment_method')
    date_pay=data.get('date_pay')
    bill_type = data.get('bill_type')  # 'appointment' or 'surgery'

    if amount is None or payment_method is None or bill_type not in ['appointment', 'surgery'] :
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Missing or invalid payment details'})

    # Retrieve the JWT token from the Authorization header
    auth_header = request.headers.get('Authorization')
    if not auth_header:
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Authorization token is missing'})

    if not auth_header.startswith('Bearer '):
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Invalid authorization token format'})
    auth_token = auth_header.split(" ")[1]

    # Decode the JWT token to get the patient ID
    patient_id = decode_auth_token(auth_token)
    if not patient_id:
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Invalid authorization token'})

    # Call the execute_payment function with the provided data
    response = execute_payment(bill_id, amount, payment_method, patient_id,date_pay, bill_type)

    return jsonify(response)
@app.route('/dbproj/daily/<string:date>', methods=['GET'])
def daily_summary_endpoint(date):
    # Verifica se o usuário é um assistente
    auth_header = request.headers.get('Authorization')
    if not auth_header or not auth_header.startswith('Bearer '):
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Invalid authorization token format'})

    # Decodifica o token JWT para obter o ID do usuário
    user_id = decode_auth_token(auth_header.split(" ")[1])
    if not user_id or not is_assistant(user_id):
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'User is not authorized to access this endpoint'})

    # Formata a data do formato de entrada
    try:
        formatted_date = datetime.strptime(date, '%Y-%m-%d').strftime('%Y-%m-%d')
    except ValueError:
        return jsonify({'status': StatusCodes['api_error'], 'errors': 'Invalid date format. Use YYYY-MM-DD'})
    conn = db_connection()
    if isinstance(conn, dict):
        return conn  # Se houver um erro na conexão, retorna a resposta de erro

    cur = conn.cursor()
    try:
        cur.execute('''
            SELECT 
            COUNT(DISTINCT h.hospid) AS hospitalizations,
            COUNT(DISTINCT surgery.surgeryid) AS surgeries,
            COUNT(DISTINCT hospitalization_prescriptions.prescriptions_presciptionsid) AS prescriptions,
            COALESCE(SUM(p.amount), 0) AS amount_spent
            FROM 
                hospitalization h
            LEFT JOIN 
                surgery ON h.hospid = surgery.hospitalization_hospid
            LEFT JOIN 
                hospitalization_prescriptions ON h.hospid = hospitalization_prescriptions.hospitalization_hospid
            LEFT JOIN 
                bill_hospitalization bh ON h.hospid = bh.hospitalization_hospid
            LEFT JOIN 
                bill ON bh.bill_billid = bill.billid
            LEFT JOIN 
                payment p ON bill.billid = p.bill_billid
            WHERE 
                h.date_start >= %s::DATE AND h.date_start < (%s::DATE + INTERVAL '1 DAY')
                or DATE(p.payment_date) = %s::DATE
''', (formatted_date, formatted_date, formatted_date))
        summary_data = cur.fetchone()
        # Formata os resultados
        results = {
            'amount_spent': float(summary_data[3]),
            'surgeries': int(summary_data[1]),
            'prescriptions': int(summary_data[2])
        }

        # Retorna os resultados no formato JSON
        return jsonify({'status': 'success', 'results': results})

    except (Exception, psycopg2.DatabaseError) as error:
        # Em caso de erro, retorna uma mensagem de erro
        return jsonify({'status': 'internal_error', 'errors': str(error)})
    finally:
        # Fecha a conexão com o banco de dados
        if conn is not None:
            conn.close()

if __name__ == '__main__':
    logging.basicConfig(filename='log_file.log', level=logging.DEBUG,
                        format='%(asctime)s [%(levelname)s]: %(message)s', datefmt='%H:%M:%S')
    logger = logging.getLogger('logger')
    logger.info('Starting the API service')

    host = '127.0.0.1'
    port = 8080
    app.run(host=host, debug=True, threaded=True, port=port)
    logger.info(f'API v1.0 online: http://{host}:{port}')