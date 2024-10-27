from flask import Flask, request, jsonify
from flask_restx import Api, Resource, fields
from index import ask

app = Flask(__name__)

swagger_app = Api(app=app)
data_apis = swagger_app.namespace('v1', version='1.0', description='Digikey Assistant APIs')

user_query_model = swagger_app.model('User Query', {
  'user_query': fields.String(required=True, description="user_query", help="user_query cannot be blank.")
})

@data_apis.route("/user_query")
class DataReport(Resource):
  @swagger_app.doc(responses={200: 'OK', 400: 'Invalid Argument', 401: "UnAuthorized", 500: 'Mapping Key Error'})
  @swagger_app.expect(user_query_model)
  def get(self):
    user_query = request.args.get('user_query')
    
    if not user_query:
      return {'message': 'user_query parameter is required.'}, 400
    
    print({'user_query': user_query})
    response = ask(user_query)

    full_response = f"ANS: {response}"
    print('response', full_response)
    return full_response

if __name__ == '__main__':
  app.run(host="0.0.0.0", port=6000, debug=True)
